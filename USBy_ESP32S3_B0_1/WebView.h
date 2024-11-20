
enum eChannel{
  Channel_1 = 1,
  Channel_2,
  Channel_3,
};

struct ChannelData {
  eChannel channel;
  String enumerator;
  float voltage;
  float current;
  int overCurrentLimit;
  int backCurrentLimit;
  bool powerControl;
  bool dataControl;

   // Setter functions
  void setEnumerator(const String& newEnumerator) { enumerator = newEnumerator; }
  void setVoltage(float newVoltage) { voltage = newVoltage; }
  void setCurrent(float newCurrent) { current = newCurrent; }
  void setOverCurrentLimit(int newOverCurrentLimit) { overCurrentLimit = newOverCurrentLimit; }
  void setBackCurrentLimit(int newBackCurrentLimit) { backCurrentLimit = newBackCurrentLimit; }
  void setPowerControl(bool newPowerControl) { powerControl = newPowerControl; }
  void setDataControl(bool newDataControl) { dataControl = newDataControl; }

  // Getter functions
  String getEnumerator() const { return enumerator; }
  float getVoltage() const { return voltage; }
  float getCurrent() const { return current; }
  int getOverCurrentLimit() const { return overCurrentLimit; }
  int getBackCurrentLimit() const { return backCurrentLimit; }
  bool getPowerControl() const { return powerControl; }
  bool getDataControl() const { return dataControl; }
};

// Standalone setters
void setChannelEnumerator(eChannel channelNumber, const String& newEnumerator);
void setChannelVoltage(eChannel channelNumber, float newVoltage);
void setChannelCurrent(eChannel channelNumber, float newCurrent);
void setChannelOverCurrentLimit(eChannel channelNumber, int newLimit);
void setChannelBackCurrentLimit(eChannel channelNumber, int newLimit);
void setChannelPowerControl(eChannel channelNumber, bool control);
void setChannelDataControl(eChannel channelNumber, bool control);

// Standalone getters
String getChannelEnumerator(eChannel channelNumber);
float getChannelVoltage(eChannel channelNumber);
float getChannelCurrent(eChannel channelNumber);
int getChannelOverCurrentLimit(eChannel channelNumber);
int getChannelBackCurrentLimit(eChannel channelNumber);
bool getChannelPowerControl(eChannel channelNumber);
bool getChannelDataControl(eChannel channelNumber);

void StartWebView();