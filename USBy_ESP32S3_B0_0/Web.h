
enum eChannel{
  Channel_1 = 0,
  Channel_2,
  Channel_3,
};

struct ChannelData {
  String enumerator;
  float voltage;
  float current;
  int overCurrentLimit;
  int backCurrentLimit;
  bool powerControl;
  bool dataControl;
};

void updateChannelEnumerator(int channelIndex, const char *newEnumerator);
void updateChannelVoltage(int channelIndex, float newVoltage);
void updateChannelCurrent(int channelIndex, float newCurrent);

void StartWifi();