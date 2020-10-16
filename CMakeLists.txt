cmake_minimum_required(VERSION 3.12)

project(bfa)

include_directories(headers sources tests)

enable_testing()

set(HEADERS
        headers/bfa.h
        headers/bv.h
        headers/vect/bv8.h
        headers/vect/bv16.h
        headers/vect/bv32.h
        headers/vect/bv64.h
        headers/vect/bvdata.h
        headers/vect/bvio.h
        headers/vect/domain.h
        headers/vect/domainmask.h
        headers/vect/chunkvector.h
		headers/vect/graysequence.h
		headers/vect/linearcombinations.h
		headers/iter/cnkiter.h
		headers/func/bfbase.h
		headers/func/bf.h
		headers/func/anf.h
		headers/func/vbfbase.h
		headers/func/vbf.h
		headers/func/vanf.h
		headers/func/fpolynomial.h
		headers/func/crypto.h
		headers/linear/linear.h
		headers/linear/gjmatrix.h
		headers/gf2/bpolynomials.h
		headers/gf2/gf2.h
		headers/gf2/gf2table.h
		headers/gf2/gf2factory.h
		headers/formula/lexem.h
		headers/formula/parsedformula.h
		headers/formula/parsedcalculator.h
		headers/integer/rational.h
		headers/props/walshspectrum.h
		headers/props/ddt.h
		headers/props/subspacebfiter.h
		headers/props/utils/cosetstack.h
		headers/props/utils/sortingmap.h
)

set(LIBSOURCES
        sources/vect/bvio.cpp
		sources/func/bfbase.cpp
		sources/func/bf.cpp
		sources/func/anf.cpp
		sources/func/vbfbase.cpp
		sources/func/vbf.cpp
		sources/func/vanf.cpp
		sources/func/fpolynomial.cpp
		sources/func/crypto.cpp
		sources/linear/gjmatrix.cpp
		sources/gf2/gf2.cpp
		sources/gf2/gf2table.cpp
		sources/gf2/gf2factory.cpp
		sources/formula/lexem.cpp
		sources/formula/parsedformula.cpp
		sources/props/walshspectrum.cpp
		sources/props/ddt.cpp
		sources/props/subspacebfiter.cpp
)

add_library(bf STATIC ${HEADERS} ${LIBSOURCES})

add_executable(booloptest tests/booloptest.cpp)
target_link_libraries(booloptest PRIVATE bf)

add_executable(chunkvectortest tests/chunkvectortest.cpp)
target_link_libraries(chunkvectortest PRIVATE bf)

add_executable(anftest tests/anftest.cpp)
target_link_libraries(anftest PRIVATE bf)

add_executable(gf2test tests/gf2test.cpp)
target_link_libraries(gf2test PRIVATE bf)

add_executable(fpolynomialtest tests/fpolynomialtest.cpp)
target_link_libraries(fpolynomialtest PRIVATE bf)

add_executable(walshtest tests/walshtest.cpp)
target_link_libraries(walshtest PRIVATE bf)

add_executable(cryptotest tests/cryptotest.cpp)
target_link_libraries(cryptotest PRIVATE bf)

add_executable(vcryptotest tests/vcryptotest.cpp)
target_link_libraries(vcryptotest PRIVATE bf)

add_executable(subspacestest tests/subspacestest.cpp)
target_link_libraries(subspacestest PRIVATE bf)

add_test(BooleanOperationTest booloptest)
add_test(ChunkVectorTest chunkvectortest)
add_test(AnfTest anftest)
add_test(GF2Test gf2test)
add_test(FPolynomialTest fpolynomialtest)
add_test(WalshTest walshtest)
add_test(CryptoPropertiesTest cryptotest)
add_test(VectorialCryptoPropertiesTest vcryptotest)
add_test(SubspacesTest subspacestest)
