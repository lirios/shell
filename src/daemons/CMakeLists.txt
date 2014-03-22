add_subdirectory(notifications)
if(PolkitQt5-1_FOUND AND QtAccountsService_FOUND)
    add_subdirectory(polkitagent)
endif()
