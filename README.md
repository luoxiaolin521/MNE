# MNE: Multi-relational Network Embedding
This is the codes and data for MNE model in our paper "[A Structural Representation Learning for Multi-relational Networks][1]".

If you would like to acknowledge our efforts, please cite the following paper:

    @inproceedings{ijcai2017-565,
    author    = {Lin Liu, Xin Li, William K. Cheung, Chengcheng Xu},
    title     = {A Structural Representation Learning for Multi-relational Networks},
    booktitle = {Proceedings of the Twenty-Sixth International Joint Conference on
                  Artificial Intelligence, {IJCAI-17}},
    pages     = {4047--4053},
    year      = {2017},
    doi       = {10.24963/ijcai.2017/565},
    url       = {https://doi.org/10.24963/ijcai.2017/565},
    }

# Prerequisites
C++

python 3

sklearn

# Usage
There are five files/folder (some may be zipped to meet the space limit):
- `MNE.cpp`: The MNE model for embedding
- `logistic_classification.py`: The classifier for classification
- `logistic_link_prediction.py`: The classifier for link prediction
- `FB15k`: The dataset case from [FreeBase][2]
- `WN18`: The dataset case from [WordNet][3]

## Run for classification
```shell
./mne -trainset data/all_train.txt -outputdir output/ -entity2id data/entity2id.txt -relation2id data/relation2id.txt
python logistic_classification.py -entityVec output/entity_vec.txt -relationVec output/relation_vec.txt -trainLink data/train_link.txt -result result/testresult_MNE.txt
```

## Run for link prediction
```shell
./mne -trainset data/train.txt -outputdir output/ -entity2id data/entity2id.txt -relation2id data/relation2id.txt
python logistic_link_prediction.py -entityVec output/entity_vec.txt -relationVec output/relation_vec.txt -trainLink data/train_link.txt -testLink data/test_link.txt -result result/testresult_MNE.txt
```

## Note
- The output files (learned embeddings) will be stored in the `-outputdir ` directory during embedding training.
- Please make the `-result` directory before run the program, and the test results will be stored in the `-result` directory.
- the `all_train.txt` in the run case means the file which contans all triples in dataset, equalling `train.txt` + `valid.txt` + `test.txt`.



[1]: https://www.ijcai.org/proceedings/2017/565
[2]: http://dl.acm.org/citation.cfm?id=1376746
[3]: http://dl.acm.org/citation.cfm?id=219748
