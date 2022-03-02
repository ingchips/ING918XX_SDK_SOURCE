#if (__cplusplus <= 201400)
#error C++ compiler is too old
#endif

#include <functional>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "platform_api.h"
#include "att_db_util.h"
#include "bluetooth.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#define printf platform_printf

#define CMD_MAX_LEN 128

extern "C" void led_ctrl(int id, int state);
extern "C" void led_toggle(int id);
extern "C" void buzz(int freq);
extern "C" float get_temperature(void);
extern "C" float get_humidity(void);
extern "C" float get_pressure(void);
extern "C" void set_led_hsl(float H, float S, float L);
extern "C" void set_led_rgb(uint8_t r, uint8_t g, uint8_t b);
extern "C" float read_adc(int channel);

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

void sleep(int ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

#define set_hsl set_led_hsl
#define set_rgb set_led_rgb

#define MAX_HANDLERS 20
typedef std::function<void(const uint8_t *buffer, uint16_t buffer_size)> att_write_callback_t;

static att_write_callback_t write_callbacks[MAX_HANDLERS];

extern "C" uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset,
                                  uint8_t * buffer, uint16_t buffer_size)
{
    switch (att_handle)
    {

    default:
        return 0;
    }
}

extern "C" int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode,
                              uint16_t offset, const uint8_t *buffer, uint16_t buffer_size)
{
    if (att_handle < MAX_HANDLERS)
    {
        if (write_callbacks[att_handle])
            write_callbacks[att_handle](buffer, buffer_size);
    }
    else;
    return 0;
}

// This function is buggy, intentionally
const uint8_t *guid_to_bytes(const char *guid)
{
    static uint8_t bytes[16];
    for (int i = 0; i < 16; i++)
    {
        bytes[i] = (uint8_t)strtoul(guid + (i * 2), NULL, 16);
    }
    return bytes;
}

class Service
{
public:
    Service(uint16_t uuid)
    {
        att_db_util_add_service_uuid16(uuid);
    }

    Service(const char *uuid)
    {
        att_db_util_add_service_uuid128(guid_to_bytes(uuid));
    }
};

class Characteristic
{
private:
    uint16_t m_handle;
public:
    Characteristic(const char *uuid, att_write_callback_t fun)
    {
        m_handle = att_db_util_add_characteristic_uuid128(
            guid_to_bytes(uuid),
            ATT_PROPERTY_INDICATE | ATT_PROPERTY_NOTIFY | ATT_PROPERTY_WRITE | ATT_PROPERTY_DYNAMIC,
            NULL, 0);
        write_callbacks[m_handle] = fun;
    }
    uint16_t Handle()
    {
        return m_handle;
    }
};

class BleAdvertisement
{
private:
    char m_name[31];
public:
    void set_name(const char *name)
    {
        strncpy(m_name, name, sizeof(m_name) - 1);
    }
    void build()
    {
        extern uint8_t adv_data[31];
        extern uint8_t adv_data_len;
        adv_data_len = 0;
        adv_data[adv_data_len++] = 0x02;
        adv_data[adv_data_len++] = 0x01;
        adv_data[adv_data_len++] = 0x06;
        adv_data[adv_data_len++] = strlen(m_name) + 1;
        adv_data[adv_data_len++] = 9;       // complete name
        memcpy(&adv_data[adv_data_len], m_name, strlen(m_name));
        adv_data_len += strlen(m_name);
    }
};

#define MAX_KEY_COUNT   32
#define MAX_LED_COUNT   32

class LEDOp
{
private:
    int m_index;
public:
    LEDOp(int index): m_index(index) {}

    void on(void)
    {
        led_ctrl(m_index, 1);
    }

    void off(void)
    {
        led_ctrl(m_index, 0);
    }

    void set(bool state)
    {
        led_ctrl(m_index, state ? 1 : 0);
    }

    void blink(void)
    {
        on();
        sleep(50);
        off();
    }

    void toggle(void)
    {
        led_toggle(m_index);
    }
};

class LED
{
public:
    LEDOp operator[] (int index)
    {
        return LEDOp(index);
    }
    void toggle(void)
    {
        for (int i = 0; i < MAX_LED_COUNT; i++)
            led_toggle(i);
    }
    void on(void)
    {
        set(true);
    }
    void off(void)
    {
        set(false);
    }
    void set(bool state)
    {
        for (int i = 0; i < MAX_LED_COUNT; i++)
            led_ctrl(i, state ? 1 : 0);
    }
    void set(uint32_t state)
    {
        for (int i = 0; i < MAX_LED_COUNT; i++)
            led_ctrl(i, (state & (1 << i)) != 0);
    }
    void blink(void)
    {
        set(true);
        sleep(50);
        set(false);
    }
    void update(std::function<bool(int index)> fun)
    {
        for (int i = 0; i < MAX_LED_COUNT; i++)
            led_ctrl(i, fun(i));
    }
    const static bool 亮 = true;
    const static bool 灭 = false;
};

class Key
{
public:
    typedef std::function<void(bool pressed)> key_changed_handler_t;
    typedef std::function<void(void)> key_event_handler_t;

    class KeyHandler
    {
    protected:
        int m_index;
    public:
        KeyHandler(int index) : m_index(index) {}
        void on_change(key_changed_handler_t handler)
        {
            if ((0 <= m_index) && (m_index < MAX_KEY_COUNT))
                Key::handlers[m_index] = handler;
        }
        void on_up(key_event_handler_t handler)
        {
            if ((0 <= m_index) && (m_index < MAX_KEY_COUNT))
                Key::up_handlers[m_index] = handler;
        }
        void on_down(key_event_handler_t handler)
        {
            if ((0 <= m_index) && (m_index < MAX_KEY_COUNT))
                Key::down_handlers[m_index] = handler;
        }
    };
public:
    static key_changed_handler_t handlers[MAX_KEY_COUNT];
    static key_event_handler_t up_handlers[MAX_KEY_COUNT];
    static key_event_handler_t down_handlers[MAX_KEY_COUNT];

    KeyHandler operator[] (int index)
    {
        return KeyHandler(index);
    }
};

Key::key_changed_handler_t Key::handlers[MAX_KEY_COUNT] = {0};
Key::key_event_handler_t   Key::up_handlers[MAX_KEY_COUNT] = {0};
Key::key_event_handler_t   Key::down_handlers[MAX_KEY_COUNT] = {0};

static LED LED;
static Key Key;

extern "C" void on_key_changed(int i, int value)
{
    if (Key::handlers[i])
        Key::handlers[i](value == 1);
    if (value == 0)
    {
        if (Key::up_handlers[i])
            Key::up_handlers[i]();
    }
    else
    {
        if (Key::down_handlers[i])
            Key::down_handlers[i]();
    }
}

typedef std::function<void(void)> void_callback_t;

extern "C" void timer_callback(TimerHandle_t xTimer)
{
    void_callback_t *cb = (void_callback_t *)pvTimerGetTimerID(xTimer);
    (*cb)();
}

void setInterval(void_callback_t fun, int interval_ms)
{
    void_callback_t *cb = new void_callback_t;
    *cb = fun;
    TimerHandle_t xTimer = xTimerCreate("t",
                pdMS_TO_TICKS(interval_ms),
                pdTRUE,
                cb,
                timer_callback);
    xTimerStart(xTimer, portMAX_DELAY);
}

extern "C" void task_entry(void * param)
 {
	void_callback_t *cb = (void_callback_t *)param;
    (*cb)();
    while (true) sleep(1000);
 }

void spawn(void_callback_t fun)
{
    void_callback_t *cb = new void_callback_t;
    *cb = fun;
    xTaskCreate(task_entry,
                "",
                512 / sizeof(uint32_t),
                cb,
                2,
                NULL);
}

void play(const char *program)
{
    const int note_freqs[] = { 261, 293, 329, 349, 392, 440, 493, };
    while (*program != '\0')
    {
        char c = *program++;
        if (('1' <= c) && (c <= '7'))
        {
            buzz(0);
            sleep(20);
            buzz(note_freqs[c - '1']);

        }
        else if (c == '-')
            ;
        else if (c == ' ')
            continue;
        else
            buzz(0);
        sleep(200);
    }
    buzz(0);
}

const char *strfinddig(const char *str)
{
    while (*str != '\0')
    {
        if (('0' <= *str) && (*str <= '9'))
            return str;
        str++;
    }
    return NULL;
}

int strtoi(const char *str)
{
    const char *num = strfinddig(str);
    return num ? atoi(num) : -1;
}

void lower_case(char *dest, const char *src)
{
    while (*src != '\0')
        *dest++ = tolower(*src++);
    *dest = '\0';
}

void print(int v)
{
    printf("%d\n", v);
}

void print(float v)
{
    printf("%.2f\n", v);
}

void print(const char *v)
{
    printf("%s\n", v);
}

class WordSpliter
{
private:
    char str[CMD_MAX_LEN];
    char *words[CMD_MAX_LEN];
    int word_count;
public:
    WordSpliter() {}
    void Split(const char *str)
    {
        lower_case(this->str, str);
        memset(words, 0, sizeof(words));
        word_count = 0;
        char *token = strtok(this->str, " ");
        while (token != NULL)
        {
            words[word_count++] = token;
            token = strtok(NULL, " ");
        }
    }
    bool Has(const char *str)
    {
        for (int i = 0; i < word_count; i++)
        {
            if (strcmp(words[i], str) == 0)
                return true;
        }
        return false;
    }
};

WordSpliter spliter;

#define HAS_VERB(x) (spliter.Has(x) || spliter.Has(x "ING"))

int 执行英语指令(const char *指令)
{
    static char cmd[CMD_MAX_LEN];
    spliter.Split(指令);

    if (spliter.Has("led") || spliter.Has("light"))
    {
        int i = strtoi(指令);
        bool on = (strstr(指令, "on") != NULL);
        bool off = (strstr(指令, "off") != NULL);

        if (on ^ off)
        {
            if (i >= 0)
            {
                if (HAS_VERB("blink") ||HAS_VERB("flash"))
                    LED[i].blink();
                else
                    LED[i].set(on);
            }
            else
                LED.set(on);
        }
        else
        {
            if (((HAS_VERB("blink") || HAS_VERB("flash"))) && (i >= 0))
            {
                LED[i].blink();
            }
            else
            {
                if (i >= 0)
                    LED[i].toggle();
                else
                    LED.toggle();
            }
        }
    }
    else if (spliter.Has("sound") || HAS_VERB("buzz"))
    {
        int i = strtoi(指令);
        buzz(i > 0 ? i : 0);
    }
    else if (spliter.Has("wait") || spliter.Has("delay") || spliter.Has("sleep"))
    {
        int interval = strtoi(指令);
        if (interval > 0)
            sleep(interval);
    }
    else
        return -1;
    return 0;
}

int 执行(const char *指令)
{
    if (strstr(指令, "灯"))
    {
        int i = strtoi(指令);
        bool on = (strstr(指令, "亮") != NULL) || (strstr(指令, "开") != NULL);
        bool off = (strstr(指令, "灭") != NULL) || (strstr(指令, "关") != NULL);

        if (on ^ off)
        {
            if ((strstr(指令, "不") != NULL) || (strstr(指令, "别") != NULL))
                on = !on;
            if (i >= 0)
            {
                if (strstr(指令, "闪"))
                    LED[i].blink();
                else
                    LED[i].set(on);
            }
            else
                LED.set(on);
        }
        else
        {
            if (strstr(指令, "闪"))
            {
                if (i >= 0)
                    LED[i].blink();
                else
                    LED.blink();
            }
            else
            {
                if (i >= 0)
                    LED[i].toggle();
                else
                    LED.toggle();
            }
        }
    }
    else if (strstr(指令, "声") || strstr(指令, "音") || strstr(指令, "鸣"))
    {
        int i = strtoi(指令);
        buzz(i > 0 ? i : 0);
    }
    else if (strstr(指令, "唱") || strstr(指令, "播") || strstr(指令, "奏"))
    {
        const char *song = strfinddig(指令);
        if (song) play(song);
    }
    else if (strstr(指令, "等") || strstr(指令, "后") || strstr(指令, "延") || strstr(指令, "休")
            || strstr(指令, "停"))
    {
        int interval = strtoi(指令);
        if (interval > 0)
        {
            if (strstr(指令, "秒") && (strstr(指令, "毫") == NULL))
                interval *= 1000;
            sleep(interval);
        }
        else
            sleep(500);
    }
    else
        return 执行英语指令(指令);
    return 0;
}

int 执行(const void *指令, int 长度)
{
    static char cmd[CMD_MAX_LEN];
    int cnt = MIN(sizeof(cmd) - 1, 长度);
    memcpy(cmd, 指令, cnt);
    cmd[cnt] = 0;
    return 执行(cmd);
}

#define exec 执行
#define set_interval            setInterval

#define 读取电压                read_adc
#define 现在的温度              get_temperature()
#define 现在的湿度              get_humidity()
#define 现在的气压              get_pressure()

#define 设置红绿蓝              set_rgb
#define 设置色饱亮              set_hsl
#define 输出                    print

#define 按键                    Key
#define 的状态改变时            on_change
#define 被按下时                on_down
#define 按下时                  on_down
#define 弹起时                  on_up

#define 开灯                    LED.set(true)
#define 关灯                    LED.set(false)
#define 灯                      LED
#define 闪一下                  blink()
#define 打开                    on()
#define 关闭                    off()
#define 控制各灯(F)             LED.update(F)

#define 演奏                    play
#define 定时动作(动作, 时间间隔) setInterval([&]() { 动作; }, (时间间隔))
#define 定时执行                setInterval
#define 一直(...)               while (1) { __VA_ARGS__; }
#define 一直执行                一直
#define 一直重复                一直
#define 重复                    一直
#define $(...)                  执行(#__VA_ARGS__)
#define 创建新线程              spawn

#define 如果                    if
#define 否则                    else
#define 变量                    auto
#define 布尔量                  bool
#define 真                      true
#define 假                      false
#define 静态                    static
#define 返回                    return
#define 等于                    ==
#define 不等于                  !=
#define 大于                    >
#define 小于                    <
#define 大于等于                >=
#define 小于等于                <=
#define 循环                    for
#define 或者                    ||
#define 且                      &&

#define 设置蓝牙名称            set_name
#define 定义服务                Service
#define 定义服务特征            Characteristic

BleAdvertisement BleAdvertisement;

#define set_name(name) BleAdvertisement.set_name(name)

extern "C" void gap_set_adv_set_random_addr(const uint8_t adv_handle, const uint8_t *addr);

extern "C" void playground_start(void)
{
    set_name("Playground");
#include "../playground/playground.cpp"
    ;
    BleAdvertisement.build();
}
