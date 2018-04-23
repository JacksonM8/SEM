#include "common.h"
#include <workers/opencl/opencl_worker.h>

#include <math.h>
#include <random>

#define PI 3.1415926535897932384
#define EPS 1e-6

/****
 * FFT testing
 ****/

/*struct Matrix{
    Matrix(size_t rows, size_t columns){
        this->rows = rows;
        this->columns = columns;
    }
    Matrix(size_t size) : Matrix(size, size){};
    Matrix() = default;

     size_t GetElements() const{
        return rows * columns;
    }

    size_t rows = 0;
    size_t columns = 0;
};*/

/*class OpenCLFFTWorkerConstructor{
    public:
        OpenCLFFTWorkerConstructor(DeviceParam device):
            component_("component"),
            worker_(component_, "openclworker")
        {
            auto platform_attr = worker_.GetAttribute("platform_id").lock();
            if (platform_attr) {
                platform_attr->set_Integer(device.platform_id);
            }
            auto device_attr = worker_.GetAttribute("device_id").lock();
            if (device_attr) {
                device_attr->set_Integer(device.device_id);
            }
        }
        Component component_;
        OpenCL_Worker worker_;
};*/


struct FFTParam{
    FFTParam(DeviceParam device, const std::vector<float>& data_in, const std::vector<float>& data_out, bool expect_success = true) {
        this->device = device;
        this->data_in = data_in;
        this->data_out = data_out;
        this->expect_success = expect_success;
    };
    
    DeviceParam device;
    std::vector<float> data_in;
    std::vector<float> data_out;
    bool expect_success = true;
};

void addFrequency(std::vector<float>& data, float frequency, float amplitude, float phase_shift) {
    for (unsigned int i=0; i<data.size(); i++) {
		data[i] += amplitude * (float)(cos((float)frequency*((float)i/data.size())*2*PI + phase_shift));
	}
}

/**
 * Constant input data should generate a single spike at the 0 bin equal to the length of the vector multiplied by the amplitude
 */
std::vector<float> generateConstantInput(size_t length, float amplitude) {
    std::vector<float> data(length);
	for (unsigned int i=0; i< data.size(); i++) {
		data[i] = amplitude;
	}
    return data;
}
std::vector<float> generateConstantOutput(size_t length, float amplitude) {
    std::vector<float> data(length, 0);
	data[0] = length * amplitude;
    return data;
}

std::vector<float> generateAlignedFrequencyInput(size_t length, float amplitude, float frequency, size_t phase_shift) {
    std::vector<float> data(length, 0);
    addFrequency(data, frequency, amplitude, (float)phase_shift);
    return data;
}

std::vector<float> generateAlignedFrequencyOutput(size_t length, float amplitude, float frequency, size_t phase_shift) {
    std::vector<float> data(length, (float)0);
    data[(size_t)abs(frequency)*2] = amplitude * (float)(length/2) * (float)cos(phase_shift);
    data[(size_t)abs(frequency)*2+1] = amplitude * (float)(length/2) * (float)sin(phase_shift);
    return data;
}

std::vector<float> generateMultipleAlignedFrequencyInput(size_t length, float frequency1, float frequency2) {
    std::vector<float> data(length, 0);
    addFrequency(data, frequency1, 1, 0);
    addFrequency(data, frequency2, 3, 0);
    return data;
}

std::vector<float> generateMultipleAlignedFrequencyOutput(size_t length, float frequency1, float frequency2) {
    std::vector<float> data(length, (float)0);
    data[(size_t)abs(frequency1)*2] = 1 * (float)(length/2);
    data[(size_t)abs(frequency2)*2] = 3 * (float)(length/2);
    return data;
}

std::ostream& operator<<(std::ostream& os, const FFTParam& f) {
    return os << f.device << ", length: " << f.data_in.size() << " - input data: " << ::testing::PrintToString(f.data_in) << " , output data: " << ::testing::PrintToString(f.data_out);
};

class FFTFixture: public ::testing::TestWithParam<FFTParam>, public OpenCL_WorkerConstructor{
    public:
        FFTFixture() : OpenCL_WorkerConstructor(GetParam().device){
            if(!worker_.Configure()){
                throw std::runtime_error("Failed to configure worker in FFTFixture constructor");
            }
        }
};

TEST_P(FFTFixture, FFTtest)
{
    auto data = GetParam().data_in;
    auto& expected_output = GetParam().data_out;
    auto expect_success = GetParam().expect_success;

    // Make sure that test case params are valid in terms of size
    ASSERT_EQ(data.size(), expected_output.size());

    // as a constant vector has been passed in, we can pull amplitude out of the first element
    float amplitude = data[0];

	bool did_succeed = worker_.FFT(data);

    /*if (expected_output[0] != 0) {
        for (const auto& val : data) {
            std::cout << val << ", ";
        }
        std::cerr << std::endl;
    }*/

    ASSERT_EQ(did_succeed, expect_success);

    ASSERT_EQ(data.size(), expected_output.size());

    // Check that output is the sum of the amplitudes for constant testsB
    //EXPECT_NEAR_RELATIVE(data[0], amplitude * data.size(), EPS);
	
    std::cout << data[3] << ", " << expected_output[3] << std::endl;
    EXPECT_FLOATS_NEARLY_EQ(data, expected_output, 1e-5)
}

typedef std::tuple<std::vector<float>, std::vector<float>, bool> TestData;

/**
 * Permutes all test cases across all devices
 */
std::vector<FFTParam> permuteFFTTests(std::vector<DeviceParam> devices, std::vector<TestData> testcases) {
    std::vector<FFTParam> permuted_params;
    for (auto& device : devices) {
        for (auto& testcase : testcases) {
            permuted_params.emplace_back(device, std::get<0>(testcase), std::get<1>(testcase), std::get<2>(testcase));
        }
    }
    return permuted_params;
}

std::vector<FFTParam> getConstantTests() {
    std::vector<FFTParam> params;
    std::vector<TestData> tests;
    tests.emplace_back(generateConstantInput(4, 1.0), generateConstantOutput(4, 1.0), true);
    tests.emplace_back(generateConstantInput(4, 7.0), generateConstantOutput(4, 7.0), true);
    tests.emplace_back(generateConstantInput(8, 1.0), generateConstantOutput(8, 1.0), true);
    tests.emplace_back(generateConstantInput(8, 7.0), generateConstantOutput(8, 7.0), true);
    tests.emplace_back(generateConstantInput(8, -7.0), generateConstantOutput(8, 7.0), true);
    tests.emplace_back(generateConstantInput(2048, 1.0), generateConstantOutput(2048, 1.0), true);
    tests.emplace_back(generateConstantInput(2048, 7.0), generateConstantOutput(2048, 7.0), true);
    return permuteFFTTests(getDevices(), tests);
}

std::vector<FFTParam> getSingleAlignedTests() {
    std::vector<FFTParam> params;
    std::vector<TestData> tests;
    tests.emplace_back(generateAlignedFrequencyInput(4, 1, 1, 0), generateAlignedFrequencyOutput(4, 1, 1, 0), true);
    tests.emplace_back(generateAlignedFrequencyInput(4, 2, 1, 0), generateAlignedFrequencyOutput(4, 2, 1, 0), true);
    tests.emplace_back(generateAlignedFrequencyInput(4, 2, 2, 0), generateAlignedFrequencyOutput(4, 2, 2, 0), true);
    tests.emplace_back(generateAlignedFrequencyInput(4, 1, 1, 1), generateAlignedFrequencyOutput(4, 1, 1, 1), true);
    tests.emplace_back(generateAlignedFrequencyInput(16, 1, 1, 0), generateAlignedFrequencyOutput(16, 1, 1, 0), true);
    tests.emplace_back(generateAlignedFrequencyInput(16, 2, 3, 0), generateAlignedFrequencyOutput(16, 2, 3, 0), true);
    tests.emplace_back(generateAlignedFrequencyInput(16, -2, 3, 0), generateAlignedFrequencyOutput(16, 2, 3, 0), true);
    tests.emplace_back(generateAlignedFrequencyInput(16, 2, -3, 0), generateAlignedFrequencyOutput(16, 2, 3, 0), true);
    //tests.emplace_back(generateAlignedFrequencyInput(4096, 1, 1, 0), generateAlignedFrequencyOutput(4096, 1, 1, 0), true);
    //tests.emplace_back(generateAlignedFrequencyInput(4096, 2, 61, 0), generateAlignedFrequencyOutput(4096, 2, 61, 0), true);
    return permuteFFTTests(getDevices(), tests);
}

std::vector<FFTParam> getMultipleAlignedTests() {
    std::vector<FFTParam> params;
    std::vector<TestData> tests;
    tests.emplace_back(generateMultipleAlignedFrequencyInput(16, 1, 2), generateMultipleAlignedFrequencyOutput(16, 1, 2), true);
    tests.emplace_back(generateMultipleAlignedFrequencyInput(32, 1, 3), generateMultipleAlignedFrequencyOutput(32, 1, 3), true);
    tests.emplace_back(generateMultipleAlignedFrequencyInput(64, 5, -7), generateMultipleAlignedFrequencyOutput(64, 5, -7), true);
    return permuteFFTTests(getDevices(), tests);
}

std::vector<FFTParam> getUnalignedTests() {
    std::vector<FFTParam> params;
    std::vector<TestData> tests;
    tests.emplace_back(generateAlignedFrequencyInput(16, 1, 2.5, 0), generateAlignedFrequencyOutput(16, 1, 2.5, 0), true);
    tests.emplace_back(generateAlignedFrequencyInput(16, 1, 1, 0), generateAlignedFrequencyOutput(16, 1, 1, 0), true);
    return permuteFFTTests(getDevices(), tests);
}




INSTANTIATE_TEST_CASE_P(Constant, FFTFixture, ::testing::ValuesIn(getConstantTests()));
INSTANTIATE_TEST_CASE_P(SingleAlignedFrequency, FFTFixture, ::testing::ValuesIn(getSingleAlignedTests()));
INSTANTIATE_TEST_CASE_P(MultipleAlignedFrequency, FFTFixture, ::testing::ValuesIn(getMultipleAlignedTests()));
INSTANTIATE_TEST_CASE_P(DISABLED_UnalignedFrequencies, FFTFixture, ::testing::ValuesIn(getUnalignedTests()));