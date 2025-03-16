#!/bin/bash

echo ""
export LD_LIBRARY_PATH="./my_release/x86/lib/:LD_LIBRARY_PATH"
for test_bin in $(ls "./my_release/x86" | grep "^test_*"); do
    if [[ -f "./my_release/x86/${test_bin}" ]]; then
        echo "Running ***********************[${test_bin}]***********************"
        if ! ./my_release/x86/${test_bin}; then
            echo ""
            echo "Test case [${test_bin}] run failed!"
            exit -1
        fi
    fi
done

echo "All test cases have been run completely!"
