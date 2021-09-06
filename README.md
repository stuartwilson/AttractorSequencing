# AttractorSequencing

Evolving recurrent networks to store sequences of patterns

Clone morphologica into the enclosing directory and then do the usual 

```mkdir build
cd build
cmake ..
make 
cd ..
```

Run the model and inspect the results using

```./build/model config.json logs 1
cat logs/log.txt
python analysis.py logs/data/h5
```

Enjoy!

