idf_component_register(
    
    SRCS            "main.c"
                    "UserInterface/userInterface.c"
                    "UserInterface/led/ledDriver.c"
                    "UserInterface/laserController.c"
                    
                    "Config/myShell_cfg.c"
                    "Config/ledDriver_cfg.c"

                    "Lib/myShell/src/myShell.c"

                    "HWI/shellComUART.c"
                    "HWI/adcController.c"

                    "Sensors/temperatureMonitoring.c"
                    "Sensors/pwrMonitoring.c"
                    "Sensors/tempConversion.c"
                    "Sensors/sensorController.c"
                    
                    "Alarm/alarmController.c"

                    "ThermalManagement/thermalManagement.c"

    PRIV_REQUIRES   spi_flash
                    driver
                    
    INCLUDE_DIRS    "."
                    "UserInterface"
                    "UserInterface/led"
                    "Config"
                    "Lib/myShell/include"
                    "HWI"
                    "Sensors"
                    "Alarm"
                    "ThermalManagement"
)

component_compile_options(-Wno-error=format= -Wno-format)