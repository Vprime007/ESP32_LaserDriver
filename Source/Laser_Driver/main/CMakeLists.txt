idf_component_register(
    
    SRCS            "main.c"
                    "UserInterface/userInterface.c"
                    
                    "Config/myShell_cfg.c"

                    "Lib/myShell/src/myShell.c"

                    "BSP/shellComUART.c"
                    
    PRIV_REQUIRES   spi_flash
                    driver
                    
    INCLUDE_DIRS    "."
                    "UserInterface"
                    "Config"
                    "Lib/myShell/include"
                    "BSP"
)

component_compile_options(-Wno-error=format= -Wno-format)