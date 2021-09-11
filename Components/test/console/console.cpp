#include <test/console/console.h>
#include <console/driver.h>
#include <console/log.h>

#include <math.h>
#define PI 3.14159265358979323846

void test::Console::init()
{

    for (int i=0;i<400;i++)
    {
        double v = ((double)i/200) * PI;
        int16_t sine = (int16_t)(sin(v)*512.0);
        int16_t cosine = (int16_t)(cos(v)*512.0);
        sine_[i] = sine + 512;
        cosine_[i] = cosine + 512;
    }

    plotTask_.start(20); //50Hz
    oscilloscopeTask_.start(1);
    strandTask_.start(1000);
}

M_EVENT_HANDLER(test::Console, testStrand)
{
	LOG_PRINT("This event is over Strand");
	myStrandStrand.done(0);
}

M_TASK_HANDLER(test::Console, strand)
{
	myStrandStrand.post(&testStrandEvent, &finishedEvent);
}

M_EVENT_HANDLER(test::Console, finished, uint8_t)
{
	LOG_PRINT("Event Finished");
}

M_EVENT_HANDLER(test::Console,toggle)
{
    /*TODO: led toggle here*/
}

U_ACTION_HANDLER(test::Console, start)
{
    plotTask_.start(20);

    //toggleTask.start(interval_);
    console::Driver::instance().sendPacket(test::Console::Started,0,nullptr);
    console::Controller::instance().print("Blink Started");
}

U_ACTION_HANDLER(test::Console, stop)
{
    plotTask_.stop();
	/* Test stick stuck with stick delay > timeout of WDT */
	//core::Engine::instance().delay(1500);
    //toggleTask.stop();
    console::Driver::instance().sendPacket(test::Console::Started,0,nullptr);
    console::Controller::instance().print("Blink Stopped");
}

U_INTEGER_HANDLER(test::Console, interval)
{
    interval_ = value;
    toggleTask_.start(interval_);
    console::Controller::instance().printf("Interval=%d", interval_);
}

U_TEXT_HANDLER(test::Console, name)
{
    for (int i=0;i<length;i++) name_[i] = data[i];
    name_[length] = 0;
    console::Controller::instance().printf("Name=%s", name_);
}

U_ACTION_HANDLER(test::Console, hello)
{
    console::Controller::instance().printf("Hello %s", name_);
}

U_ACTION_HANDLER(test::Console,sync)
{
    console::Controller::instance().updateIntField(Interval, interval_);
    console::Controller::instance().updateTextField(Name, name_);
}

M_EVENT_HANDLER(test::Console, plot)
{
    static uint32_t angle=0;
    console::Controller::instance().plot(0, sine_[angle]);
    console::Controller::instance().plot(1, cosine_[angle]);
    if (++angle>=400) angle=0;
}

M_EVENT_HANDLER(test::Console, oscilloscope)
{
    static uint32_t angle=0;
    //singlePlot(sine_[angle] + rand()%10);
    //dualPlot(sine_[angle] + rand()%10, cosine_[angle] + rand()%10);
    //triplePlot(sine_[angle] + rand()%10, cosine_[angle] + rand()%10, cosine_[angle]/2 + rand()%5);
    quadPlot(sine_[angle] + rand()%10, cosine_[angle] + rand()%10, cosine_[angle]/2 + rand()%5, sine_[angle]/2 + rand()%5);
    if (++angle>=400) angle=0;
}
