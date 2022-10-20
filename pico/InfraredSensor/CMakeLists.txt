add_executable(infraredMain
            infraredMain.c
        )

# pull in common dependencies
target_link_libraries(infraredMain pico_stdlib)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(infraredMain)
