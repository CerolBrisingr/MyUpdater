# If OpenSSL is not there, we won't expect useful certificates either
find_program(OPENSSL_EXECUTABLE openssl REQUIRED)
message(STATUS "OpenSSL version: ${OPENSSL_VERSION}")
message(STATUS "OpenSSL executable: ${OPENSSL_EXECUTABLE}")

file(MAKE_DIRECTORY "${CERT_FOLDER}/ca" "${CERT_FOLDER}/server")

# Place script file that cleans up old/broken certificates
set(CLEAN_SCRIPT_CONTENT [=[
message(STATUS ${ASSETS_TO_CHECK})
# We expect a list of triplets "PathToCert|PathToKey|CertName;PathToCert.."
foreach(ITEM IN LISTS ASSETS_TO_CHECK)
    # Change triplet seperator to make it readable. Split up triplet
    string(REPLACE "|" ";" DATA "${ITEM}")
    list(GET DATA 0 C_PATH)
    list(GET DATA 1 K_PATH)
    list(GET DATA 2 C_NAME)

    if(EXISTS "${C_PATH}" AND EXISTS "${K_PATH}")
        execute_process(
            COMMAND "${OPENSSL_EXE}" x509 -checkend 172800 -noout -in "${C_PATH}"
            RESULT_VARIABLE res
            OUTPUT_QUIET ERROR_QUIET
        )
            
        execute_process(COMMAND "${OPENSSL_EXE}" rsa -noout -modulus -in "${K_PATH}" OUTPUT_VARIABLE K_MOD)
        execute_process(COMMAND "${OPENSSL_EXE}" x509 -noout -modulus -in "${C_PATH}" OUTPUT_VARIABLE C_MOD)

        if(NOT res EQUAL 0 OR NOT "${K_MOD}" STREQUAL "${C_MOD}")
            message(STATUS "[SSL] ${C_NAME} invalid/expired. Deleting to trigger regeneration.")
            file(REMOVE "${C_PATH}")
        else()
            message(STATUS "[SSL] ${C_NAME} is valid.")
        endif()
    endif()
endforeach()
]=])
set(CLEAN_SCRIPT_FILE "${CMAKE_CURRENT_BINARY_DIR}/cleanup_certs.cmake")
file(WRITE "${CLEAN_SCRIPT_FILE}" "${CLEAN_SCRIPT_CONTENT}")

set(CERT_FOLDER "${CMAKE_CURRENT_LIST_DIR}/certs")
set(SSL_ASSET_CA_KEY       "${CERT_FOLDER}/ca/ca.key")
set(SSL_ASSET_CA_CERT      "${CERT_FOLDER}/ca/ca.crt")
set(SSL_ASSET_SERVER_KEY   "${CERT_FOLDER}/server/server.key")
set(SSL_ASSET_SERVER_CERT  "${CERT_FOLDER}/server/server.crt")

# Building Root CA
add_custom_command(
    OUTPUT "${SSL_ASSET_CA_CERT}" "${SSL_ASSET_CA_KEY}"
    COMMAND ${OPENSSL_EXECUTABLE} req -x509 -new -noenc 
            -keyout ca.key -out ca.crt -sha256 -days 3650 
            -config "${CERT_FOLDER}/ca.conf"
    DEPENDS "${CERT_FOLDER}/ca.conf"
    WORKING_DIRECTORY "${CERT_FOLDER}/ca"
    COMMENT "Building Root CA..."
)

# Build server certificate
add_custom_command(
    OUTPUT "${SSL_ASSET_SERVER_CERT}" "${SSL_ASSET_SERVER_KEY}"
    COMMAND ${OPENSSL_EXECUTABLE} req -new -out server.csr
            -newkey rsa:2048 -noenc -keyout server.key
            -config "${CERT_FOLDER}/server.conf"
    COMMAND ${OPENSSL_EXECUTABLE} x509 -req -in server.csr 
            -CA "${SSL_ASSET_CA_CERT}" -CAkey "${SSL_ASSET_CA_KEY}" -CAcreateserial 
            -out server.crt -days 15 -sha256 
            -copy_extensions copy
    DEPENDS "${SSL_ASSET_CA_CERT}" "${SSL_ASSET_CA_KEY}" "${CERT_FOLDER}/server.conf"
    BYPRODUCTS "${CERT_FOLDER}/ca/ca.srl" "${CERT_FOLDER}/server/server.csr"
    WORKING_DIRECTORY "${CERT_FOLDER}/server"
    COMMENT "Building Server Certificate..."
)

# Gather needed assets
set(MY_ASSET_LIST
    "${SSL_ASSET_CA_CERT}|${SSL_ASSET_CA_KEY}|CA-Certificate"
    "${SSL_ASSET_SERVER_CERT}|${SSL_ASSET_SERVER_KEY}|Server-Certificate"
)
set(ASSET_STRING "${MY_ASSET_LIST}") # Clean up to avoid interference during handover

# Define cleanup target
add_custom_target(check_ssl_expiry
    COMMAND ${CMAKE_COMMAND} 
            "-D ASSETS_TO_CHECK=${ASSET_STRING}"
            -D OPENSSL_EXE="${OPENSSL_EXECUTABLE}"
            -P "${CLEAN_SCRIPT_FILE}"
    VERBATIM
)

# Tie it all together
add_custom_target(ssl_assets ALL DEPENDS "${SSL_ASSET_SERVER_CERT}")
add_dependencies(ssl_assets check_ssl_expiry)

# TODO: Add SERVER_TIMEOUT and SERVER_NO_CA
set(SSL_ASSET_SERVER_TIMEOUT_KEY "")
set(SSL_ASSET_SERVER_TIMEOUT_CERT "")
set(SSL_ASSET_SERVER_NO_CA_KEY "")
set(SSL_ASSET_SERVER_NO_CA_CERT "")
