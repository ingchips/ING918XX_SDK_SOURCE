spawn([]() {
    auto id_on = 0;
    auto dir = 1;
    while (true) {
        id_on += dir;
        if ((id_on < 0) || (id_on > 7))
        {
            dir *= -1;
            id_on += dir;
        }

        LED.update([=](auto id) { return id == id_on; });
        sleep(50);
    }
});
