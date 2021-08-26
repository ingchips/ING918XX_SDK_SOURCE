// Service Generic Access: 1800
0x0A, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x28,
0x00, 0x18, 
// Characteristic Device Name: 2A00
0x0D, 0x00, 0x02, 0x00, 0x02, 0x00, 0x03, 0x28,
0x02, 0x03, 0x00, 0x00, 0x2A, 
0x20, 0x00, 0x02, 0x00, 0x03, 0x00, 0x00, 0x2A,
0x49, 0x4E, 0x47, 0x20, 0x56, 0x6F, 0x69, 0x63, 
0x65, 0x20, 0x52, 0x65, 0x6D, 0x6F, 0x74, 0x65, 
0x20, 0x43, 0x6F, 0x6E, 0x74, 0x72, 0x6F, 0x6C, 

// Service INGChips Voice Output Service: {00000001-494e-4743-4849-505355554944}
0x18, 0x00, 0x02, 0x00, 0x04, 0x00, 0x00, 0x28,
0x44, 0x49, 0x55, 0x55, 0x53, 0x50, 0x49, 0x48, 
0x43, 0x47, 0x4E, 0x49, 0x01, 0x00, 0x00, 0x00, 
// Characteristic Voice Control: {bf83f3f1-399a-414d-9035-ce64ceb3ff67}
0x1B, 0x00, 0x02, 0x00, 0x05, 0x00, 0x03, 0x28,
0x06, 0x06, 0x00, 0x67, 0xFF, 0xB3, 0xCE, 0x64, 
0xCE, 0x35, 0x90, 0x4D, 0x41, 0x9A, 0x39, 0xF1, 
0xF3, 0x83, 0xBF, 
0x16, 0x00, 0x06, 0x03, 0x06, 0x00,
0x67, 0xFF, 0xB3, 0xCE, 0x64, 0xCE, 0x35, 0x90, 
0x4D, 0x41, 0x9A, 0x39, 0xF1, 0xF3, 0x83, 0xBF, 
// Characteristic Voice Output: {bf83f3f2-399a-414d-9035-ce64ceb3ff67}
0x1B, 0x00, 0x02, 0x00, 0x07, 0x00, 0x03, 0x28,
0x10, 0x08, 0x00, 0x67, 0xFF, 0xB3, 0xCE, 0x64, 
0xCE, 0x35, 0x90, 0x4D, 0x41, 0x9A, 0x39, 0xF2, 
0xF3, 0x83, 0xBF, 
0x16, 0x00, 0x10, 0x02, 0x08, 0x00,
0x67, 0xFF, 0xB3, 0xCE, 0x64, 0xCE, 0x35, 0x90, 
0x4D, 0x41, 0x9A, 0x39, 0xF2, 0xF3, 0x83, 0xBF, 
// Descriptor Client Characteristic Configuration: 2902
0x0A, 0x00, 0x0A, 0x01, 0x09, 0x00, 0x02, 0x29,
0x00, 0x00, 
// Characteristic Voice Information: {10000001-494e-4743-4849-505355554944}
0x1B, 0x00, 0x02, 0x00, 0x0A, 0x00, 0x03, 0x28,
0x02, 0x0B, 0x00, 0x44, 0x49, 0x55, 0x55, 0x53, 
0x50, 0x49, 0x48, 0x43, 0x47, 0x4E, 0x49, 0x01, 
0x00, 0x00, 0x10, 
0x17, 0x00, 0x02, 0x02, 0x0B, 0x00,
0x44, 0x49, 0x55, 0x55, 0x53, 0x50, 0x49, 0x48, 
0x43, 0x47, 0x4E, 0x49, 0x01, 0x00, 0x00, 0x10, 
0x96, 

0x00,0x00
// total size = 239

// HANDLE_DEVICE_NAME=3
// HANDLE_VOICE_CONTROL=6
// HANDLE_VOICE_OUTPUT=8
// HANDLE_VOICE_OUTPUT_CLIENT_CHAR_CONFIG=9
// HANDLE_VOICE_INFORMATION=11

// HANDLE_DEVICE_NAME_OFFSET=31
// HANDLE_VOICE_CONTROL_OFFSET=128
// HANDLE_VOICE_OUTPUT_OFFSET=177
// HANDLE_VOICE_OUTPUT_CLIENT_CHAR_CONFIG_OFFSET=185
// HANDLE_VOICE_INFORMATION_OFFSET=236
