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

# Usage
We provide two cases for MNE:
1. FB15K: [FreeBase][2]
2. WN18: [WordNet][3]

and there are six files for each case (zipped the data to meet the space limit):
- `MNE.cpp`: The MNE model for embedding
- `logistic-link-prediction.py`: Link prediction by the embedded network
- `logistic-triplet-classification.py`: Triplet classification by the embedded network
- `data for embedding.zip`: The dataset for embedding training
- `data for link prediction.zip`: The dataset for link prediction
- `data for triplet classification.zip`: The dataset for triplet classification

# Note
The output files will be stored in `./result ` directory during embedding training.


[1]: https://www.ijcai.org/proceedings/2017/565
[2]: http://dl.acm.org/citation.cfm?id=1376746
[3]: http://dl.acm.org/citation.cfm?id=219748
