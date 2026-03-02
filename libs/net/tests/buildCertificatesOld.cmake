# If OpenSSL is not there, we won't expect useful certificates either
find_program(OPENSSL_EXECUTABLE openssl REQUIRED)
message(STATUS "OpenSSL version: ${OPENSSL_VERSION}")
message(STATUS "OpenSSL executable: ${OPENSSL_EXECUTABLE}")

set(CERT_FOLDER "${CMAKE_CURRENT_LIST_DIR}/certs")

# Test if all certificate files are in place
set(CERTS_EXIST TRUE)
foreach(FILE "ca/ca.crt" "server/server.crt" "server/server.key")
	if(NOT EXISTS "${CERT_FOLDER}/${FILE}")
		set(CERTS_EXIST FALSE)
		break()
	endif()
endforeach()

# Test for validity
set(CERTS_VALID FALSE)
if(CERTS_EXIST)
	message(STATUS "Certificate files found, testing validity")
	# Implement here

    # 1. Zeit-Check (Gültig für die nächsten 2 Tage?)
    execute_process(
        COMMAND ${OPENSSL_EXECUTABLE} x509 -checkend 172800 -noout -in "${CERT_FOLDER}/server/server.crt"
        RESULT_VARIABLE CERT_EXPIRED
        OUTPUT_QUIET ERROR_QUIET
    )

    # 2. Match-Check (Passen Key und Cert zusammen?)
    execute_process(COMMAND ${OPENSSL_EXECUTABLE} rsa -noout -modulus -in "${CERT_FOLDER}/server/server.key" OUTPUT_VARIABLE KEY_MOD)
    execute_process(COMMAND ${OPENSSL_EXECUTABLE} x509 -noout -modulus -in "${CERT_FOLDER}/server/server.crt" OUTPUT_VARIABLE CRT_MOD)

    # Nur wenn beides passt, markieren wir sie als valide
    if(CERT_EXPIRED EQUAL 0 AND "${KEY_MOD}" STREQUAL "${CRT_MOD}")
        set(CERTS_VALID TRUE)
        message(STATUS "Certs are fine. Skipping generation.")
    endif()
endif()

# Create certs if needed
if(NOT CERTS_VALID)
	message(STATUS "Missing valid certificate files. Building new ones")
    # Clean up directory
    #FILE(REMOVE_RECURSE ${CERT_FOLDER})
    FILE(MAKE_DIRECTORY "${CERT_FOLDER}/ca")
    FILE(MAKE_DIRECTORY "${CERT_FOLDER}/server")

    # TODO: split in individual directories, apply config (-config, -extensions)

    # Root CA, create key on the fly (-newkey line)
    execute_process(
        COMMAND ${OPENSSL_EXECUTABLE} req -x509 
            -newkey rsa:2048 -noenc -keyout ca.key 
            -out ca.crt -sha256 -days 3650
            -config ../ca.conf
        WORKING_DIRECTORY ${CERT_FOLDER}/ca)

    # Server-Cert with SAN
    #file(WRITE "${CERT_FOLDER}server.ext" "subjectAltName = DNS:localhost, IP:127.0.0.1") 
    execute_process(
        COMMAND ${OPENSSL_EXECUTABLE} req -new
            -newkey rsa:2048 -noenc -keyout server.key
            -out server.csr
            -config ../server.conf
        WORKING_DIRECTORY ${CERT_FOLDER}/server)
    execute_process(
        COMMAND ${OPENSSL_EXECUTABLE} x509 -req
            -in server.csr -CA "../ca/ca.crt" -CAkey "../ca/ca.key" -CAcreateserial
            -out server.crt -days 15 -sha256
            -copy_extensions copy
        WORKING_DIRECTORY ${CERT_FOLDER}/server)

    # Check for completeness
    foreach(FILE "ca/ca.crt" "server/server.crt" "server/server.key")
        if(NOT EXISTS "${CERT_FOLDER}/${FILE}")
            message(FATAL_ERROR "Critical error: Certificate file ${FILE} could not be created!")
        endif()
    endforeach()
endif()

# TODO: Add SERVER_TIMEOUT and SERVER_NO_CA
set(SSL_ASSET_CA_CERT "${CERT_FOLDER}/ca/ca.crt")
set(SSL_ASSET_SERVER_KEY "${CERT_FOLDER}/server/server.key")
set(SSL_ASSET_SERVER_CERT "${CERT_FOLDER}/server/server.cache")
set(SSL_ASSET_SERVER_TIMEOUT_KEY "")
set(SSL_ASSET_SERVER_TIMEOUT_CERT "")
set(SSL_ASSET_SERVER_NO_CA_KEY "")
set(SSL_ASSET_SERVER_NO_CA_CERT "")
