# If OpenSSL is not there, we won't expect useful certificates either
find_program(OPENSSL_EXECUTABLE openssl REQUIRED)

set(CERT_FOLDER "${CMAKE_CURRENT_LIST_DIR}/certs")

# Test if all certificate files are in place
set(CERTS_EXIST TRUE)
foreach(FILE "ca.crt" "server.crt" "server.key")
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
        COMMAND ${OPENSSL_EXECUTABLE} x509 -checkend 172800 -noout -in "${CERT_FOLDER}/server.crt"
        RESULT_VARIABLE CERT_EXPIRED
        OUTPUT_QUIET ERROR_QUIET
    )

    # 2. Match-Check (Passen Key und Cert zusammen?)
    execute_process(COMMAND ${OPENSSL_EXECUTABLE} rsa -noout -modulus -in "${CERT_FOLDER}/server.key" OUTPUT_VARIABLE KEY_MOD)
    execute_process(COMMAND ${OPENSSL_EXECUTABLE} x509 -noout -modulus -in "${CERT_FOLDER}/server.crt" OUTPUT_VARIABLE CRT_MOD)

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
    FILE(REMOVE_RECURSE ${CERT_FOLDER})
    FILE(MAKE_DIRECTORY ${CERT_FOLDER})

    # Root CA
    execute_process(COMMAND ${OPENSSL_EXECUTABLE} genrsa -out ca.key 2048 WORKING_DIRECTORY ${CERT_FOLDER})
    execute_process(COMMAND ${OPENSSL_EXECUTABLE} req -x509 -new -nodes -key ca.key -sha256 -days 10 -out ca.crt -subj "/CN=Test-Root-CA" WORKING_DIRECTORY ${CERT_FOLDER})

    # Server-Cert mit SAN
    file(WRITE "${CERT_FOLDER}/server.ext" "subjectAltName = DNS:localhost, IP:127.0.0.1")
    execute_process(COMMAND ${OPENSSL_EXECUTABLE} genrsa -out server.key 2048 WORKING_DIRECTORY ${CERT_FOLDER})
    execute_process(COMMAND ${OPENSSL_EXECUTABLE} req -new -key server.key -out server.csr -subj "/CN=localhost" WORKING_DIRECTORY ${CERT_FOLDER})
    execute_process(COMMAND ${OPENSSL_EXECUTABLE} x509 -req -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out server.crt -days 10 -sha256 -extfile server.ext WORKING_DIRECTORY ${CERT_FOLDER})

    # Check for completeness
    foreach(FILE "ca.crt" "server.crt" "server.key")
        if(NOT EXISTS "${CERT_FOLDER}/${FILE}")
            message(FATAL_ERROR "Kritischer Fehler: Zertifikatsdatei ${FILE} konnte nicht erstellt werden!")
        endif()
    endforeach()
endif()

# TODO: Add SERVER_TIMEOUT and SERVER_NO_CA
set(SSL_ASSET_CA_CERT "${CERT_FOLDER}/ca.crt")
set(SSL_ASSET_SERVER_KEY "${CERT_FOLDER}/server.key")
set(SSL_ASSET_SERVER_CERT "${CERT_FOLDER}/server.cache")
set(SSL_ASSET_SERVER_TIMEOUT_KEY "")
set(SSL_ASSET_SERVER_TIMEOUT_CERT "")
set(SSL_ASSET_SERVER_NO_CA_KEY "")
set(SSL_ASSET_SERVER_NO_CA_CERT "")
