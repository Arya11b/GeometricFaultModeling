#ifndef RAW_PARSER
#define RAW_PARSER

#include <filesystem>
#include <array>
#include <iostream>
#include <Eigen/dense>

static inline int ipow(int base, int exp) {
    int result = 1;
    for (;;)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        if (!exp)
            break;
        base *= base;
    }

    return result;
}

// parser is not generalized for data with dimension ratios other than 1:1...1
template<typename Scalar, typename DataType, int _Dim>
class RawParser
{
private:
    std::array<int, _Dim> dimension;
    int dataSize = 1;
    std::unique_ptr<DataType> data;
    void allocateData() {
        data.reset(new DataType[dataSize]);
    }

    typedef Eigen::Matrix<Scalar, Eigen::Dynamic, _Dim>	Eigen::MatrixX3d;
    typedef Eigen::Matrix<Scalar, 1, _Dim>	Eigen::RowVector3d;

    Eigen::MatrixX3d pointCloud;
public:
    const Eigen::MatrixX3d& getPointCloud() {
        return pointCloud;
    }
    void set111Dimension(int size) {
        for (int i = 0; i < dimension.size(); i++)
        {
            dimension[i] = size;
            dataSize *= size;
        }
        
    }
    void initArrayFromVolFile( const char* fileName)
    {
        allocateData();
        std::FILE* file = fopen(fileName, "rb");
        if (file == NULL)
        {
            printf("open the file failed\n");
        }
        fread(data.get(), sizeof(DataType), dataSize, file);
        fclose(file);
    }

    // extracts the data to the point cloud using Eigen
    void processData(float spacing, Eigen::RowVector3d pos=Eigen::RowVector3d::Zero()) {
        int dataPoints{};
        for (int i = 0; i < dataSize; i++)
        {
            if (data.get()[i]) dataPoints++;
        }
        pointCloud.resize(dataPoints, _Dim);
        int pcIndex{};
        for (int i = 0; i < dataSize; i++)
        {
            if (data.get()[i]) {
                //for (int d = 0; d < _Dim; d++)
                //{
                //    pointCloud(pcIndex, d) = ((i % ipow(dimension[d], d + 1)) / ipow(dimension[d], d));
                //}
                pointCloud(pcIndex, 0) = i % dimension[0];
                pointCloud(pcIndex, 1) = (i % (dimension[0] * dimension[1]))/dimension[1];
                pointCloud(pcIndex, 2) = i / (dimension[1] * dimension[0]);
                pcIndex++;
            }
        }
        pointCloud *= spacing;
        //pointCloud += origin.replicate(pointCloud.rows(), 1);
    }

};

#endif