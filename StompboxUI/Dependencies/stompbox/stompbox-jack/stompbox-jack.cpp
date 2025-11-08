#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <jack/jack.h>
#include <jack/midiport.h>

#include "PluginProcessor.h"
#include "SysExMapper.h"

jack_port_t **input_ports;
jack_port_t **output_ports;
jack_port_t *midi_input_port;
jack_port_t *midi_output_port;
jack_client_t *client;
PluginProcessor *guitarProcessor;
SysExMapper sysExMapper;

bool haveSentMidiStartMessage = false;
unsigned char midiStartMessage[15] = { 0xF0, 0x41, 0x00, 0x00, 0x00, 0x00, 0x30, 0x12,
 0x7F, 0x00, 0x00, 0x01, 0x01, 0x7F, 0xF7 };
std::list<MidiEvent> midiEvents;

static void signal_handler(int sig)
{
    (void)sig;

    jack_client_close(client);
    fprintf(stderr, "signal received, exiting ...\n");
    exit(0);
}

/**
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
 *
 * This client follows a simple rule: when the JACK transport is
 * running, copy the input port to the output.  When it stops, exit.
 */

size_t bufferSize = 0;

int process(jack_nframes_t nframes, void* arg)
{
    (void)arg;

    guitarProcessor->ReportDSPLoad(jack_cpu_load(client) / 100);

    //if (!haveSentMidiStartMessage)
    //{
    //    void* port_buf = jack_port_get_buffer(midi_output_port, nframes);
    //    jack_midi_clear_buffer(port_buf);

    //    unsigned char* buffer = jack_midi_event_reserve(port_buf, 0, sizeof(midiStartMessage));
    //    memcpy(buffer, midiStartMessage, sizeof(midiStartMessage));

    //    haveSentMidiStartMessage = true;
    //}

    if (jack_port_connected(midi_input_port) > 0)
    {
        void* port_buf = jack_port_get_buffer(midi_input_port, nframes);

        jack_midi_event_t in_event;
        jack_nframes_t event_count = jack_midi_get_event_count(port_buf);

        if (event_count > 0)
        {
            for (jack_nframes_t i = 0; i < event_count; i++)
            {
                jack_midi_event_get(&in_event, port_buf, i);

                fprintf(stderr, "Midi Msg:");

                for (size_t b = 0; b < in_event.size; b++)
                {
                    fprintf(stderr, " %x", in_event.buffer[b]);
                }

                fprintf(stderr, "\n");

                //if (sysExMapper.HandleMidiMessage(in_event.buffer, in_event.size, midiEvents))
                //{
                //    for (const auto& event : midiEvents)
                //    {
                //        fprintf(stderr, "Command: %d  Data1: %d  Data2: %d\n", event.MidiCommand, event.MidiData1, event.MidiData2);

                //        int data2 = event.MidiData2;

                //        if (event.MidiData2MaxValue != 0)
                //        {
                //            data2 = (int)(((double)data2 / (double)event.MidiData2MaxValue) * 127);
                //        }

                //        guitarProcessor->HandleMidiCommand(event.MidiCommand, event.MidiData1, data2);
                //    }

                //    midiEvents.clear();
                //}
                //else
                //{
                    int commandCode = (in_event.buffer[0] & 0xF0);
                    int channel = (in_event.buffer[0] & 0x0F) + 1;

                    fprintf(stderr, "Midi Channel: %d Command: %d  Data1: %d  Data2: %d\n", channel, commandCode, in_event.buffer[1], in_event.buffer[2]);

                    guitarProcessor->HandleMidiCommand(commandCode, in_event.buffer[1], in_event.buffer[2]);
                //}
            }
        }
    }

    auto in = (jack_default_audio_sample_t*)jack_port_get_buffer(input_ports[0], nframes);

    auto out = (jack_default_audio_sample_t*)jack_port_get_buffer(output_ports[0], nframes);

    guitarProcessor->Process(in, out, nframes);

    auto out2 = (jack_default_audio_sample_t*)jack_port_get_buffer(output_ports[1], nframes);

    for (unsigned int samp = 0; samp < nframes; samp++)
    {
        out2[samp] = out[samp];
    }

    return 0;
}

/**
 * JACK calls this shutdown_callback if the server ever shuts down or
 * decides to disconnect the client.
 */
void jack_shutdown(void *arg )
{
    (void)arg;

    jack_free(input_ports);
    jack_free(output_ports);

    exit(1);
}

int main(int argc, char* argv[])
{
    const char** ports;
    const char* client_name;
    const char* preset_name = NULL;
    const char* server_name = NULL;
    jack_options_t options = JackNullOption;
    jack_status_t status;

    client_name = "stompbox";

    if (argc > 1)        /* client name specified? */
    {
        preset_name = argv[1];
    }

    fprintf(stderr, "%s\n\n", guitarProcessor->GetVersion().c_str());

    // Old stuff for working with BOSS GT-1 sysex messages
    
    // Expression pedal
    SysExCC cc;
    cc.SysExData[0] = 0x60; cc.SysExData[1] = 0x00; cc.SysExData[2] = 0x06; cc.SysExData[3] = 0x2a;
    cc.MidiCommand = ControlChange;
    cc.MidiData = 32;
    cc.MaxValue = 100;
    sysExMapper.AddCC(cc);

    // Down switch
    cc.SysExData[0] = 0x60; cc.SysExData[1] = 0x00; cc.SysExData[2] = 0x01; cc.SysExData[3] = 0x40;
    cc.MidiCommand = ControlChange;
    cc.MidiData = 33;
    sysExMapper.AddCC(cc);

    // Up Switch
    cc.SysExData[0] = 0x60; cc.SysExData[1] = 0x00; cc.SysExData[2] = 0x00; cc.SysExData[3] = 0x35;
    cc.MidiCommand = ControlChange;
    cc.MidiData = 34;
    sysExMapper.AddCC(cc);

    // CTL1 Switch
    cc.SysExData[0] = 0x60; cc.SysExData[1] = 0x00; cc.SysExData[2] = 0x00; cc.SysExData[3] = 0x5b;
    cc.MidiCommand = ControlChange;
    cc.MidiData = 35;
    sysExMapper.AddCC(cc);

    cc.SysExData[0] = 0x00; cc.SysExData[1] = 0x01; cc.SysExData[2] = 0x00; cc.SysExData[3] = 0x00; cc.SysExData[4] = 0x00;
    cc.DataLength = 5;
    cc.MidiCommand = PatchChange;
    cc.MidiData = -1;
    sysExMapper.AddCC(cc);


    /* open a client connection to the JACK server */

    client = jack_client_open(client_name, options, &status, server_name);
    if (client == NULL)
    {
        fprintf(stderr, "jack_client_open() failed, "
            "status = 0x%2.0x\n", status);
        if (status & JackServerFailed)
        {
            fprintf(stderr, "Unable to connect to JACK server\n");
        }
        exit(1);
    }

    if (status & JackServerStarted)
    {
        fprintf(stderr, "JACK server started\n");
    }

    if (status & JackNameNotUnique)
    {
        client_name = jack_get_client_name(client);
        fprintf(stderr, "unique name `%s' assigned\n", client_name);
    }

    /* tell the JACK server to call `process()' whenever
       there is work to be done.
    */

    jack_set_process_callback(client, process, 0);

    /* tell the JACK server to call `jack_shutdown()' if
       it ever shuts down, either entirely, or if it
       just decides to stop calling us.
    */

    jack_on_shutdown(client, jack_shutdown, 0);

    /* create two ports pairs*/
    input_ports = (jack_port_t**)calloc(2, sizeof(jack_port_t*));
    output_ports = (jack_port_t**)calloc(2, sizeof(jack_port_t*));

    char port_name[16];

    for (int i = 0; i < 2; i++)
    {
        sprintf(port_name, "input_%d", i + 1);
        input_ports[i] = jack_port_register(client, port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
        sprintf(port_name, "output_%d", i + 1);
        output_ports[i] = jack_port_register(client, port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

        if ((input_ports[i] == NULL) || (output_ports[i] == NULL))
        {
            fprintf(stderr, "no more JACK ports available\n");
            exit(1);
        }
    }

    midi_input_port = jack_port_register(client, "midi_in", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
    //midi_output_port = jack_port_register(client, "midi_out", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);

    guitarProcessor = new PluginProcessor(std::filesystem::current_path(), false);
    guitarProcessor->Init((float)jack_get_sample_rate(client));

    guitarProcessor->HandleCommand("SetGlobalChain MasterChain MasterIn Chain Input Slot Amp Slot Tonestack Chain FxLoop Slot Cabinet Chain Output MasterChain MasterOut");

    guitarProcessor->HandleCommand("SetChain MasterIn AudioFileRecorder Tuner Input");
    guitarProcessor->HandleCommand("SetChain MasterOut AudioFilePlayer Master");

    guitarProcessor->HandleCommand("SetPluginSlot Amp NAM");
    guitarProcessor->HandleCommand("SetPluginSlot Tonestack EQ-7");
    guitarProcessor->HandleCommand("SetPluginSlot Cabinet Cabinet");

    if (preset_name != NULL)
    {
        guitarProcessor->LoadPreset(preset_name);
    }

    guitarProcessor->StartServer();

    /* Tell the JACK server that we are ready to roll.  Our
     * process() callback will start running now. */

    if (jack_activate(client))
    {
        fprintf(stderr, "cannot activate client");
        exit(1);
    }

    /* Connect the ports.  You can't do this before the client is
     * activated, because we can't make connections to clients
     * that aren't running.  Note the confusing (but necessary)
     * orientation of the driver backend ports: playback ports are
     * "input" to the backend, and capture ports are "output" from
     * it.
     */

    ports = jack_get_ports(client, NULL, NULL, JackPortIsPhysical | JackPortIsOutput);

    if (ports == NULL)
    {
        fprintf(stderr, "no physical capture ports\n");
        exit(1);
    }

    int numAudioInputPorts = 0;

    while (ports[numAudioInputPorts] != NULL)
        numAudioInputPorts++;

    fprintf(stderr, "%d audio input ports found\n", numAudioInputPorts);

    if (numAudioInputPorts < 1)
    {
        jack_free(ports);

        fprintf(stderr, "Less than 1 audio input ports\n");
        exit(1);
    }

    for (int i = 0; i < 1; i++)
    {
        fprintf(stderr, "connecting to input port [%s]\n", ports[i]);

        if (jack_connect(client, ports[i], jack_port_name(input_ports[i])))
            fprintf(stderr, "cannot connect port\n");
    }

    jack_free(ports);

    ports = jack_get_ports(client, NULL, NULL, JackPortIsPhysical | JackPortIsInput);

    if (ports == NULL)
    {
        fprintf(stderr, "no physical playback ports\n");
        exit(1);
    }

    int numAudioOutputPorts = 0;

    while (ports[numAudioOutputPorts] != NULL)
        numAudioOutputPorts++;

    fprintf(stderr, "%d audio output ports found\n", numAudioInputPorts);

    if (numAudioOutputPorts < 2)
    {
        jack_free(ports);

        fprintf(stderr, "Less than 2 audio output ports\n");
        exit(1);
    }

    for (int i = 0; i < 2; i++)
    {
        fprintf(stderr, "connecting to output port [%s]\n", ports[i]);

        if (jack_connect(client, jack_port_name(output_ports[i]), ports[i]))
            fprintf(stderr, "cannot connect port\n");
    }

    jack_free(ports);

    ports = jack_get_ports(client, NULL, JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput);

    if (ports == NULL)
    {
        fprintf(stderr, "no midi output port\n");
    }
    else
    {
        int numMIDIInputPorts = 0;

        while (ports[numMIDIInputPorts] != NULL)
            numMIDIInputPorts++;

        fprintf(stderr, "%d midi input ports found\n", numMIDIInputPorts);

        for (int i = 0; i < numMIDIInputPorts; i++)
        {
            fprintf(stderr, "connecting to midi input port [%s]\n", ports[i]);

            if (jack_connect(client, ports[i], jack_port_name(midi_input_port)))
            {
                fprintf(stderr, "cannot connect port\n");
            }
        }

        jack_free(ports);

            //ports = jack_get_ports(client, NULL, JACK_DEFAULT_MIDI_TYPE, JackPortIsInput);
            //if (ports == NULL)
            //{
            //    fprintf(stderr, "no midi input port\n");
            //}
            //else
            //{
            //    if (jack_connect(client, jack_port_name(midi_output_port), ports[1]))
            //    {
            //        fprintf(stderr, "cannot connect midi output port\n");
            //    }
            //}

    }


    /* install a signal handler to properly quits jack client */
#ifdef WIN32
    signal(SIGINT, signal_handler);
    signal(SIGABRT, signal_handler);
    signal(SIGTERM, signal_handler);
#else
    signal(SIGQUIT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGHUP, signal_handler);
    signal(SIGINT, signal_handler);
#endif

    /* keep running until the transport stops */

    fprintf(stderr, "stompbox is now running\n");

    while (1)
    {
#ifdef WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
    }

    jack_client_close(client);

    delete guitarProcessor;

    exit(0);
}
