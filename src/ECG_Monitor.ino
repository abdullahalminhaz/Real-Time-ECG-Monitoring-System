/***************************************************************************
 * Project : Real-Time ECG Monitor (Part 1A)
 * Hardware:
 *      - Arduino Nano (ATmega328P)
 *      - AD8232 ECG Module
 *
 * Connections
 * -----------------------------------------
 * AD8232 OUT   -> A0
 * AD8232 LO+   -> D10
 * AD8232 LO-   -> D11
 * LED          -> D13
 *
 * Serial Baud  : 115200
 * Sampling Rate: 250 Hz (4 ms)
 *
 * Part 1A Features
 * -----------------------------------------
 * ✔ Pin configuration
 * ✔ Sampling timing variables
 * ✔ Moving Average Filter
 * ✔ Baseline Wander Removal
 * ✔ Global ECG variables
 ***************************************************************************/

//======================================================================
// PIN DEFINITIONS
//======================================================================

// Analog ECG output from AD8232
const byte ECG_PIN = A0;

// Lead-off detection pins
const byte LO_PLUS = 10;
const byte LO_MINUS = 11;

// Built-in LED used later as heartbeat indicator
const byte LED_PIN = 13;


//======================================================================
// SAMPLING CONFIGURATION
//======================================================================

// ECG should be sampled at approximately 250 Hz.
// One sample every 4 milliseconds.
const unsigned long SAMPLE_PERIOD = 4;

// Stores the previous sampling time.
unsigned long previousSample = 0;


//======================================================================
// RAW ECG VARIABLES
//======================================================================

// Raw ADC value directly from AD8232
int rawECG = 0;

// Output after Moving Average Filter
int filteredECG = 0;

// Final processed ECG after baseline removal
int ecg = 0;


//======================================================================
// MOVING AVERAGE FILTER
//======================================================================
//
// Purpose:
// --------
// Removes high-frequency noise from ECG.
//
// Formula:
// Filtered = Sum(last N samples) / N
//
// Larger FILTER_SIZE
//      smoother waveform
//      slower response
//
// Smaller FILTER_SIZE
//      faster response
//      less filtering
//
// Recommended:
//      8 samples
//======================================================================

const byte FILTER_SIZE = 8;

// Circular buffer storing previous samples
int filterBuffer[FILTER_SIZE];

// Current index of circular buffer
byte filterIndex = 0;

// Running sum of samples
long filterSum = 0;


//======================================================================
// BASELINE WANDER REMOVAL
//======================================================================
//
// ECG often drifts slowly because of
//
// • Breathing
// • Electrode movement
// • Skin impedance
//
// This variable estimates the slowly changing baseline.
//
// ECG = Signal - Baseline
//======================================================================

long baseline = 0;


//======================================================================
// MOVING AVERAGE FILTER FUNCTION
//======================================================================

int movingAverage(int value)
{
    // Remove oldest value from running sum
    filterSum -= filterBuffer[filterIndex];

    // Store newest sample
    filterBuffer[filterIndex] = value;

    // Add newest sample
    filterSum += value;

    // Advance circular buffer index
    filterIndex++;

    // Wrap around when reaching end
    if (filterIndex >= FILTER_SIZE)
        filterIndex = 0;

    // Return average
    return filterSum / FILTER_SIZE;
}


//======================================================================
// BASELINE REMOVAL FUNCTION
//======================================================================
//
// A simple exponential moving average estimates
// the baseline.
//
// baseline(n)
// = 63/64 previous
// + 1/64 current
//
// The filtered ECG is:
//
// ECG = Signal − Baseline
//======================================================================

int removeBaseline(int value)
{
    baseline = (baseline * 63 + value) / 64;

    return value - baseline;
}
/******************************************************************************
 * PART 2A
 * Heartbeat Detection Variables
 ******************************************************************************/
 /******************************************************************************
 * SIGNAL QUALITY VARIABLES
 ******************************************************************************/

// Maximum acceptable ECG amplitude after filtering.
// Values above this are likely motion artifacts or electrical noise.
const int MAX_SIGNAL_AMPLITUDE = 900;

// Number of consecutive noisy samples.
int noisySamples = 0;

// True when the signal is considered unreliable.
bool signalValid = true;

// Number of consecutive noisy samples before declaring the signal invalid.
const int MAX_NOISY_SAMPLES = 25;

/******************************************************************************
 * REFRACTORY PERIOD
 *
 * Ignore any new peak for 250 ms after a valid heartbeat.
 ******************************************************************************/

// 250 ms refractory period
const unsigned long REFRACTORY_PERIOD = 300;

// Time when the last valid beat was detected
unsigned long lastPeakDetected = 0;
//======================================================================
// R-PEAK DETECTION VARIABLES
//======================================================================

// Adaptive threshold
int threshold = 80;

// Highest recent ECG peak
int peakValue = 0;

// Indicates whether we are currently inside a detected beat
bool peakDetected = false;


//======================================================================
// TIMING VARIABLES
//======================================================================

// Time of the previous heartbeat
unsigned long lastBeatTime = 0;

// Time between two consecutive beats
unsigned long rrInterval = 0;


//======================================================================
// HEART RATE
//======================================================================

// Current heart rate
int bpm = 0;


//======================================================================
// VALID RR LIMITS
//
// Prevents false peaks.
//
// RR < 300 ms  → ignored (>200 BPM)
// RR > 2000 ms → ignored (<30 BPM)
//======================================================================

const unsigned long MIN_RR = 400;
const unsigned long MAX_RR = 1800;


//======================================================================
// RR INTERVAL BUFFER
//
// Used to smooth BPM calculation.
//======================================================================

const byte RR_BUFFER_SIZE = 5;

unsigned long rrBuffer[RR_BUFFER_SIZE];

byte rrIndex = 0;

bool rrBufferFilled = false;

/******************************************************************************
 * DIGITAL FILTER VARIABLES
 ******************************************************************************/

// Previous raw sample
float hpPrevInput = 0.0;

// Previous high-pass output
float hpPrevOutput = 0.0;

// Previous low-pass output
float lpPrevOutput = 0.0;

/******************************************************************************
 * HEART RATE VARIABILITY
 ******************************************************************************/

// Store last 20 RR intervals
const byte HRV_BUFFER_SIZE = 20;

unsigned long hrvRR[HRV_BUFFER_SIZE];

byte hrvIndex = 0;

bool hrvFull = false;

// HRV Results
float averageRR = 0;

float SDNN = 0;

/******************************************************************************
 * Part 1B
 * ---------------------------------------------------------------------------
 * Main program
 * Performs:
 *   1. Initializes hardware
 *   2. Samples ECG at 250Hz
 *   3. Detects lead-off condition
 *   4. Reads ADC
 *   5. Applies filtering
 *   6. Sends data to Serial Plotter
 ******************************************************************************/

//======================================================================
// SETUP
//======================================================================

void setup()
{
    // Initialize Serial Communication
    // Use 115200 baud for smooth Serial Plotter updates.
    Serial.begin(115200);

    // ECG Analog Input
    pinMode(ECG_PIN, INPUT);

    // Lead-off detection pins
    pinMode(LO_PLUS, INPUT);
    pinMode(LO_MINUS, INPUT);

    // Heartbeat indicator LED
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Initialize moving-average buffer
    for (byte i = 0; i < FILTER_SIZE; i++)
    {
        filterBuffer[i] = 0;
    }

    Serial.println("====================================");
    Serial.println(" AD8232 ECG Monitor Started");
    Serial.println(" Sampling Rate : 250 Hz");
    Serial.println("====================================");
}


//======================================================================
// LOOP
//======================================================================

/******************************************************************************
 * LOOP
 ******************************************************************************/

void loop()
{
    //----------------------------------------------------------------------
    // Maintain 250 Hz Sampling Rate
    //----------------------------------------------------------------------

    unsigned long currentTime = millis();

    if ((currentTime - previousSample) < SAMPLE_PERIOD)
        return;

    previousSample = currentTime;

    //----------------------------------------------------------------------
    // Check Lead-Off Condition
    //----------------------------------------------------------------------

    bool leadOff =
        digitalRead(LO_PLUS) == HIGH ||
        digitalRead(LO_MINUS) == HIGH;

    if (leadOff)
    {
        // Reset detection state
        peakDetected = false;
        peakValue = 0;

        // Turn OFF heartbeat LED
        digitalWrite(LED_PIN, LOW);

        // Send zero values to Serial Plotter
        Serial.println("0,0,0,0");

        return;
    }

    // Read ECG
    rawECG = analogRead(ECG_PIN);

    // Moving Average
    filteredECG = movingAverage(rawECG);

    // High-pass Filter
    float hpSignal = highPassFilter(filteredECG);

    // Low-pass Filter
    float lpSignal = lowPassFilter(hpSignal);

    // Final ECG
    ecg = (int)lpSignal;

    //----------------------------------------------------------------------
    // Step 3 : Detect Heartbeat
    //----------------------------------------------------------------------

    // Check Signal Quality first
    checkSignalQuality(ecg);

    if (!signalValid)
    {
        Serial.print(ecg);
        Serial.print(",");
        Serial.println(0);

        return;
    }

    // Detect Heartbeat
    detectHeartbeat(ecg);

    //----------------------------------------------------------------------
    // Step 5 : Print Data
    //
    // Format:
    // Processed ECG,
    // BPM
    //----------------------------------------------------------------------
    Serial.print("ECG:");
    Serial.print(ecg);

    Serial.print(",");

    Serial.print("BPM:");
    Serial.print(bpm);
    Serial.print("\n");
}
/******************************************************************************
 * detectHeartbeat()
 *
 * Detects R-peaks using an adaptive threshold.
 ******************************************************************************/
/******************************************************************************
 * detectHeartbeat()
 *
 * Adaptive R-Peak Detection
 ******************************************************************************/

/******************************************************************************
 * Improved QRS Detection
 ******************************************************************************/

void detectHeartbeat(int signal)
{
    static int previousSignal = 0;
    static int previousSlope = 0;

    int slope = signal - previousSignal;

    //------------------------------------------------------------
    // Update adaptive peak estimate
    //------------------------------------------------------------

    if (signal > peakValue)
        peakValue = signal;

    peakValue = peakValue * 995 / 1000;      // Slow decay

    threshold = peakValue * 50 / 100;        // 50% threshold

    unsigned long now = millis();

    //------------------------------------------------------------
    // Refractory Period
    //------------------------------------------------------------

    if ((now - lastPeakDetected) < REFRACTORY_PERIOD)
    {
        previousSignal = signal;
        previousSlope = slope;
        return;
    }

    //------------------------------------------------------------
    // Peak Detection
    // Rising slope followed by falling slope
    //------------------------------------------------------------

    bool isPeak =
        (previousSlope > 0) &&
        (slope <= 0);

    if (isPeak &&
        signal > threshold &&
        signal > 25)          // Ignore tiny peaks
    {

        rrInterval = now - lastBeatTime;

        if (lastBeatTime != 0 &&
            rrInterval >= MIN_RR &&
            rrInterval <= MAX_RR)
        {

            //----------------------------------------------------
            // Store RR
            //----------------------------------------------------

            rrBuffer[rrIndex] = rrInterval;

            rrIndex++;

            if (rrIndex >= RR_BUFFER_SIZE)
            {
                rrIndex = 0;
                rrBufferFilled = true;
            }

            hrvRR[hrvIndex] = rrInterval;

            hrvIndex++;

            if (hrvIndex >= HRV_BUFFER_SIZE)
            {
                hrvIndex = 0;
                hrvFull = true;
            }

            //----------------------------------------------------
            // Average RR
            //----------------------------------------------------

            byte count =
                rrBufferFilled ?
                RR_BUFFER_SIZE :
                rrIndex;

            unsigned long total = 0;

            for (byte i = 0; i < count; i++)
                total += rrBuffer[i];

            if (count > 0)
            {
                unsigned long avgRR = total / count;

                if (avgRR > 0)
                    bpm = 60000UL / avgRR;
            }
        }

        //----------------------------------------------------
        // Save Beat
        //----------------------------------------------------

        lastBeatTime = now;
        lastPeakDetected = now;

        digitalWrite(LED_PIN, HIGH);
    }
    else
    {
        digitalWrite(LED_PIN, LOW);
    }

    previousSignal = signal;
    previousSlope = slope;
}

/******************************************************************************
 * checkSignalQuality()
 *
 * Rejects very large spikes that are usually caused by
 * motion artifacts or poor electrode contact.
 ******************************************************************************/

void checkSignalQuality(int signal)
{
    // Detect unusually large amplitudes
    if (abs(signal) > MAX_SIGNAL_AMPLITUDE)
    {
        noisySamples++;

        if (noisySamples >= MAX_NOISY_SAMPLES)
        {
            signalValid = false;
        }
    }
    else
    {
        // Signal returned to normal
        noisySamples = 0;
        signalValid = true;
    }
}
/******************************************************************************
 * High-Pass Filter
 *
 * Removes slow baseline drift caused by breathing
 * and electrode movement.
 ******************************************************************************/

float highPassFilter(float input)
{
    // Alpha ≈ 0.995 for 250 Hz sampling
    const float alpha = 0.995;

    float output =
        alpha *
        (hpPrevOutput + input - hpPrevInput);

    hpPrevInput = input;
    hpPrevOutput = output;

    return output;
}

/******************************************************************************
 * Low-Pass Filter
 *
 * Removes high-frequency noise.
 ******************************************************************************/

float lowPassFilter(float input)
{
    // Simple exponential low-pass
    const float alpha = 0.45;

    lpPrevOutput =
        lpPrevOutput +
        alpha * (input - lpPrevOutput);

    return lpPrevOutput;
}


