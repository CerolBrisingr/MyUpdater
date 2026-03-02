# TODO: Check for Key validity?
# TODO: Maybe "inline" this script
# This file will be parsed at build time
set(CERTS_TO_CHECK "${SSL_ASSET_CA_CERT}" "${SSL_ASSET_SERVER_CERT}")

foreach(CERT IN LISTS CERTS_TO_CHECK)
    if(EXISTS "${CERT}")
        execute_process(
            COMMAND ${OPENSSL_EXE} x509 -checkend 86400 -noout -in "${CERT}"
            RESULT_VARIABLE res
        )
        if(NOT res EQUAL 0)
            file(REMOVE "${CERT}")
        endif()
    endif()
endforeach()