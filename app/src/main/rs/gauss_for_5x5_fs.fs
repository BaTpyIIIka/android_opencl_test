#pragma version(1)
#pragma rs java_package_name(ru.michaelilyin.alg1)

static const float k[25] = { 0.0030f, 0.0133f, 0.0219f, 0.0133f, 0.0030f,
                            0.0133f, 0.0596f, 0.0983f, 0.0596f, 0.0133f,
                            0.0219f, 0.0983f, 0.1621f, 0.0983f, 0.0219f,
                            0.0133f, 0.0596f, 0.0983f, 0.0596f, 0.0133f,
                            0.0030f, 0.0133f, 0.0219f, 0.0133f, 0.0030f };
                         	
rs_allocation in;
int size;

uchar4 __attribute__((kernel)) make_gauss(uint32_t x, uint32_t y) {
	float4 f4out = { 0, 0, 0, 0 };

    if (x < 5 || y < 5 || x > size - 6 || y > size - 6) {
        return convert_uchar4(f4out);
    }

	float4 p00 = convert_float4(rsGetElementAt_uchar4(in, x - 2, y - 2)) * k[0];
    float4 p01 = convert_float4(rsGetElementAt_uchar4(in, x - 1, y - 2)) * k[1];
    float4 p02 = convert_float4(rsGetElementAt_uchar4(in, x + 0, y - 2)) * k[2];
    float4 p03 = convert_float4(rsGetElementAt_uchar4(in, x + 1, y - 2)) * k[3];
    float4 p04 = convert_float4(rsGetElementAt_uchar4(in, x + 2, y - 2)) * k[4];

    float4 p10 = convert_float4(rsGetElementAt_uchar4(in, x - 2, y - 1)) * k[5];
    float4 p11 = convert_float4(rsGetElementAt_uchar4(in, x - 1, y - 1)) * k[6];
    float4 p12 = convert_float4(rsGetElementAt_uchar4(in, x + 0, y - 1)) * k[7];
    float4 p13 = convert_float4(rsGetElementAt_uchar4(in, x + 1, y - 1)) * k[8];
    float4 p14 = convert_float4(rsGetElementAt_uchar4(in, x + 2, y - 1)) * k[9];

    float4 p20 = convert_float4(rsGetElementAt_uchar4(in, x - 2, y + 0)) * k[10];
    float4 p21 = convert_float4(rsGetElementAt_uchar4(in, x - 1, y + 0)) * k[11];
    float4 p22 = convert_float4(rsGetElementAt_uchar4(in, x + 0, y + 0)) * k[12];
    float4 p23 = convert_float4(rsGetElementAt_uchar4(in, x + 1, y + 0)) * k[13];
    float4 p24 = convert_float4(rsGetElementAt_uchar4(in, x + 2, y + 0)) * k[14];

    float4 p30 = convert_float4(rsGetElementAt_uchar4(in, x - 2, y + 1)) * k[15];
    float4 p31 = convert_float4(rsGetElementAt_uchar4(in, x - 1, y + 1)) * k[16];
    float4 p32 = convert_float4(rsGetElementAt_uchar4(in, x + 0, y + 1)) * k[17];
    float4 p33 = convert_float4(rsGetElementAt_uchar4(in, x + 1, y + 1)) * k[18];
    float4 p34 = convert_float4(rsGetElementAt_uchar4(in, x + 2, y + 1)) * k[19];

    float4 p40 = convert_float4(rsGetElementAt_uchar4(in, x - 2, y + 2)) * k[20];
    float4 p41 = convert_float4(rsGetElementAt_uchar4(in, x - 1, y + 2)) * k[21];
    float4 p42 = convert_float4(rsGetElementAt_uchar4(in, x + 0, y + 2)) * k[22];
    float4 p43 = convert_float4(rsGetElementAt_uchar4(in, x + 1, y + 2)) * k[23];
    float4 p44 = convert_float4(rsGetElementAt_uchar4(in, x + 2, y + 2)) * k[24];

    float4 outClr = p00 + p01 + p02 + p03 + p04
                  + p10 + p11 + p12 + p13 + p14
                  + p20 + p21 + p22 + p23 + p24
                  + p30 + p31 + p32 + p33 + p34
                  + p40 + p41 + p42 + p43 + p44;

    return convert_uchar4(outClr);
}