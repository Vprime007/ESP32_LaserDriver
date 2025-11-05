idf_component_register(
    
    SRCS            "main.c"
                    "UserInterface/userInterface.c"
                    
    PRIV_REQUIRES   spi_flash
                    

    INCLUDE_DIRS    "UserInterface"
)
