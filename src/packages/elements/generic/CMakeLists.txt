add_subdirectory(launcher)
if(QtAccountsService_FOUND)
    hawaiishell_install_package(element org.hawaii.elements.indicators indicators)
endif()
