idf_component_register(
    
    SRCS            "main.c"
                    "UserInterface/userInterface.c"
                    "UserInterface/led/ledDriver.c"
                    
                    "Config/myShell_cfg.c"
                    "Config/ledDriver_cfg.c"

                    "Lib/myShell/src/myShell.c"

                    "HWI/shellComUART.c"
                    "HWI/adcController.c"
                    
    PRIV_REQUIRES   spi_flash
                    driver
                    
    INCLUDE_DIRS    "."
                    "UserInterface"
                    "UserInterface/led"
                    "Config"
                    "Lib/myShell/include"
                    "HWI"
)

component_compile_options(-Wno-error=format= -Wno-format)