for i in {1..10}
    do
        ./build/model config.json logs/log$i $i &
    done
