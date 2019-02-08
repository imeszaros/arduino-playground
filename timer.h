class Timer {

public:

	Timer(unsigned long interval);

	void setOrigin(unsigned long origin);
	void reset(unsigned long interval);
	float progress(bool constrain);
	bool fire();

private:

	unsigned long _origin;
	unsigned long _interval;

	unsigned long _millis();
};
