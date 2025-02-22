#include <fastforest.h>

#include <fstream>
#include <iostream>
#include <cmath>
#include <sstream>

const fastforest::FeatureType tolerance = 1e-4;
const std::size_t nSamples = 100;
typedef float RefPredictionType;
typedef fastforest::FastForest FF;

#define CHECK_CLOSE(val, ref, tol)         \
    if (std::abs((val - ref) / ref) > tol) \
        return 1;

void fillFeaturesFive(std::vector<std::string>& features) {
    features.reserve(5);
    features.push_back("f0");
    features.push_back("f1");
    features.push_back("f2");
    features.push_back("f3");
    features.push_back("f4");
}

int exampleTest() {
    std::vector<std::string> features;
    fillFeaturesFive(features);

    const FF fastForest = fastforest::load_txt("continuous/model.txt", features);

    std::vector<fastforest::FeatureType> input;
    input.reserve(5);
    input.push_back(0.0);
    input.push_back(0.2);
    input.push_back(0.4);
    input.push_back(0.6);
    input.push_back(0.8);

    fastforest::FeatureType score = fastForest(input.data());
    fastforest::FeatureType logistcScore = 1. / (1. + std::exp(-score));

    return 0;
}

int basicTest() {
    std::vector<std::string> features;
    fillFeaturesFive(features);

    const FF fastForest = fastforest::load_txt("continuous/model.txt", features);

    std::ifstream fileX("continuous/X.csv");
    std::ifstream filePreds("continuous/preds.csv");

    std::vector<fastforest::FeatureType> input(5);
    fastforest::FeatureType score;
    RefPredictionType ref;

    for (std::size_t i = 0; i < nSamples; ++i) {
        for (std::size_t j = 0; j < input.size(); ++j) {
            fileX >> input[j];
        }
        score = fastForest(input.data());
        filePreds >> ref;

        CHECK_CLOSE(score, ref, tolerance);
    }

    return 0;
}

int softmaxTest() {
    std::vector<std::string> features;
    fillFeaturesFive(features);

    const FF fastForest = fastforest::load_txt("softmax/model.txt", features, 3);

    std::ifstream fileX("softmax/X.csv");
    std::ifstream filePreds("softmax/preds.csv");

    std::vector<fastforest::FeatureType> input(5);
    fastforest::FeatureType score;
    RefPredictionType ref;

    for (std::size_t i = 0; i < nSamples; ++i) {
        for (std::size_t j = 0; j < input.size(); ++j) {
            fileX >> input[j];
        }
        std::vector<float> output = fastForest.softmax(input.data());
        for (std::size_t j = 0; j < output.size(); ++j) {
            filePreds >> ref;
            CHECK_CLOSE(output[j], ref, tolerance);
        }
    }

    return 0;
}

// This test covers the case of trees with a single leaf node.
int softmaxNSamples100NFeatures100Test() {
    std::vector<std::string> features;
    for (std::size_t i = 0; i < 100; ++i) {
        std::stringstream ss;
        ss << "f" << i;
        features.push_back(ss.str());
    }

    const FF fastForest = fastforest::load_txt("softmax_n_samples_100_n_features_100/model.txt", features, 3);

    std::ifstream fileX("softmax_n_samples_100_n_features_100/X.csv");
    std::ifstream filePreds("softmax_n_samples_100_n_features_100/preds.csv");

    std::vector<fastforest::FeatureType> input(features.size());
    fastforest::FeatureType score;
    RefPredictionType ref;

    for (std::size_t i = 0; i < nSamples; ++i) {
        for (std::size_t j = 0; j < input.size(); ++j) {
            fileX >> input[j];
        }
        std::vector<float> output = fastForest.softmax(input.data());
        for (std::size_t j = 0; j < output.size(); ++j) {
            filePreds >> ref;
            CHECK_CLOSE(output[j], ref, tolerance);
        }
    }

    return 0;
}

#if __cplusplus >= 201103L

int softmaxArrayTest() {
    std::vector<std::string> features;
    fillFeaturesFive(features);

    const FF fastForest = fastforest::load_txt("softmax/model.txt", features, 3);

    std::ifstream fileX("softmax/X.csv");
    std::ifstream filePreds("softmax/preds.csv");

    std::vector<fastforest::FeatureType> input(5);
    fastforest::FeatureType score;
    RefPredictionType ref;

    for (std::size_t i = 0; i < nSamples; ++i) {
        for (auto& x : input) {
            fileX >> x;
        }
        for (auto& x : fastForest.softmax<3>(input.data())) {
            filePreds >> ref;
            CHECK_CLOSE(x, ref, tolerance);
        }
    }

    return 0;
}

#endif

int serializationTest() {
    {
        std::vector<std::string> features;
        fillFeaturesFive(features);
        const FF fastForest = fastforest::load_txt("continuous/model.txt", features);
        fastForest.write_bin("continuous/forest.bin");
    }

    const FF fastForest = fastforest::load_bin("continuous/forest.bin");

    std::ifstream fileX("continuous/X.csv");
    std::ifstream filePreds("continuous/preds.csv");

    std::vector<fastforest::FeatureType> input(5);
    fastforest::FeatureType score;
    RefPredictionType ref;

    for (std::size_t i = 0; i < nSamples; ++i) {
        for (std::size_t j = 0; j < input.size(); ++j) {
            fileX >> input[j];
        }
        score = fastForest(input.data());
        filePreds >> ref;

        CHECK_CLOSE(score, ref, tolerance);
    }

    return 0;
}

int discreteTest() {
    std::vector<std::string> features;
    fillFeaturesFive(features);

    const FF fastForest = fastforest::load_txt("discrete/model.txt", features);

    std::ifstream fileX("discrete/X.csv");
    std::ifstream filePreds("discrete/preds.csv");

    std::vector<fastforest::FeatureType> input(5);
    fastforest::FeatureType score;
    RefPredictionType ref;

    for (std::size_t i = 0; i < nSamples; ++i) {
        for (std::size_t j = 0; j < input.size(); ++j) {
            fileX >> input[j];
        }
        score = fastForest(input.data());
        filePreds >> ref;

        CHECK_CLOSE(score, ref, tolerance);
    }

    return 0;
}

int manyfeaturesTest() {
    std::vector<std::string> features;
    for (int i = 0; i < 311; ++i) {
        std::stringstream ss;
        ss << "f" << i;
        features.push_back(ss.str());
    }

    const FF fastForest = fastforest::load_txt("manyfeatures/model.txt", features);

    std::ifstream fileX("manyfeatures/X.csv");
    std::ifstream filePreds("manyfeatures/preds.csv");

    std::vector<fastforest::FeatureType> input(features.size());
    fastforest::FeatureType score;
    RefPredictionType ref;

    for (std::size_t i = 0; i < nSamples; ++i) {
        for (std::size_t j = 0; j < input.size(); ++j) {
            fileX >> input[j];
        }
        score = fastForest(input.data());
        filePreds >> ref;

        CHECK_CLOSE(score, ref, tolerance);
    }

    return 0;
}

#ifdef EXPERIMENTAL_TMVA_SUPPORT

int basicTMVAXMLTest() {
    std::vector<std::string> features;
    fillFeaturesFive(features);

    const FF fastForest = fastforest::load_tmva_xml("continuous/model.xml", features);

    std::ifstream fileX("continuous/X.csv");
    std::ifstream filePreds("continuous/preds.csv");

    std::vector<fastforest::FeatureType> input(5);
    fastforest::FeatureType score;
    RefPredictionType ref;

    for (std::size_t i = 0; i < nSamples; ++i) {
        for (std::size_t j = 0; j < input.size(); ++j) {
            fileX >> input[j];
        }
        score = fastForest(input.data());
        filePreds >> ref;

        CHECK_CLOSE(score, ref, tolerance);
    }

    return 0;
}

#endif

int main() {
    int ret = 0;

    ret += exampleTest();
    ret += basicTest();
    ret += softmaxTest();
    ret += softmaxNSamples100NFeatures100Test();
#if __cplusplus >= 201103L
    ret += softmaxArrayTest();
#endif
    ret += serializationTest();
    ret += discreteTest();
    ret += manyfeaturesTest();
#ifdef EXPERIMENTAL_TMVA_SUPPORT
    ret += basicTMVAXMLTest();
#endif

    if (ret == 0) {
        std::cout << "Tests PASSED" << std::endl;
    } else {
        std::cout << "Tests FAILED" << std::endl;
    }

    return ret != 0;
}
