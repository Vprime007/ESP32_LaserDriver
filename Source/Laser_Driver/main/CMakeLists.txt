idf_component_register(
    
    SRCS            "main.c"
                    "UserInterface/userInterface.c"
                    "UserInterface/triggerDriver.c"
                    "UserInterface/led/ledDriver.c"
                    "UserInterface/led/ledController.c"
                    "UserInterface/sequencer/sequencer.c"
                    
                    "LaserController/laserController.c"
                    
                    "Config/myShell_cfg.c"
                    "Config/ledDriver_cfg.c"
                    "Config/sequencer_cfg.c"

                    "Lib/myShell/src/myShell.c"

                    "HWI/shellComUART.c"
                    "HWI/adcController.c"

                    "Sensors/temperatureMonitoring.c"
                    "Sensors/pwrMonitoring.c"
                    "Sensors/tempConversion.c"
                    "Sensors/sensorController.c"
                    
                    "Alarm/alarmController.c"

                    "FanController/fanController.c"

    PRIV_REQUIRES   spi_flash
                    driver
                    
    INCLUDE_DIRS    "."
                    "UserInterface"
                    "UserInterface/led"
                    "UserInterface/sequencer"
                    "Config"
                    "Lib/myShell/include"
                    "HWI"
                    "Sensors"
                    "Alarm"
                    "FanController"
                    "LaserController"
)

component_compile_options(-Wno-error=format= -Wno-format)