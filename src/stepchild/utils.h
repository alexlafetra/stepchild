class StepchildUtilities{
    public:
    vector<uint8_t> getRadian(uint8_t h, uint8_t k, int a, int b, float angle) {
        float x1 = h + a * cos(radians(angle));
        float y1;
        if (angle > 180) {
            y1 = k - b * sqrt(1 - pow((x1 - h), 2) / pow(a, 2));
        }
        else {
            y1 = k + b * sqrt(1 - pow((x1 - h), 2) / pow(a, 2));
        }
        vector<uint8_t> coords = {uint8_t(x1),uint8_t(y1)};
        return coords;
    }
};

StepchildUtilities utils;