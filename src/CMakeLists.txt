add_executable(${PROJECT_NAME}_exe
    main.cpp
    core/data_generator.cpp
    protocols/arinc429/arinc429_message.cpp
    protocols/arinc429/arinc429_generator.cpp
    protocols/canj1939/canj1939_message.cpp
    protocols/canj1939/canj1939_generator.cpp
)

target_link_libraries(${PROJECT_NAME}_exe
    PRIVATE
        serial_bus_generator
        Threads::Threads
)

target_include_directories(${PROJECT_NAME}_exe
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

set_target_properties(${PROJECT_NAME}_exe 
    PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
)