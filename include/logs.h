#ifndef writeLog

    #ifdef ESP_ENV
    
        #ifndef MODULE_TAG
        #define MODULE_TAG "Blink-Speed-Switcher"
        #endif

        #define writeLog(format, ...) ESP_LOGI(MODULE_TAG, format, ##__VA_ARGS__)
    
    #else
        #define writeLog(format, ...) printf(format "\n", ##__VA_ARGS__)
    #endif

#endif