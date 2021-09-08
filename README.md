# AttractorSequencing

Evolving recurrent networks to store sequences of patterns

Clone morphologica (https://github.com/ABRG-Models/morphologica.git) into the enclosing directory and then do the usual cmake procedure:

```
mkdir build
cd build
cmake ..
make 
cd ..
```

Run the model and inspect the results using

```
./build/model config.json logs 1
cat logs/log.txt
python analysis.py logs/data/h5
```

Enjoy!

(NOTES ON RUNNING BATCH)

To run a batch of sims and collect the results do:

```./batch config.json logs 100```

(where 100 is the number of seeds to try)

Note you may need to do the following and run batch2.sh instead if there are line ending issues

```cat batch.sh | tr -d "\r" > batch2.sh```

then

```python processBatch.py logs datafile```

then you can do 

```python analysisBatch.py datafile.npz```

or download datafile.npz to process the data locally
