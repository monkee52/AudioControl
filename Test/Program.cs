using AydenIO.AudioControl;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Test {
    class Program {
        [STAThread]
        static void Main(string[] args) {
            Controller audioController = new Controller();

            AudioDevice[] devices = audioController.GetAudioDevices(DeviceType.Render, DeviceState.Active);

            foreach (AudioDevice device in devices) {
                Console.WriteLine("[" + device.Id + "]");
                Console.WriteLine("Name =          " + device.Name);
                Console.WriteLine("Friendly Name = " + device.FriendlyName);
                Console.WriteLine("Description =   " + device.Description);
                Console.WriteLine("Type =          " + device.Type.ToString());
                Console.WriteLine("State =         " + device.State.ToString());
                Console.WriteLine("Role =          " + device);
                Console.WriteLine("Mute =          " + device.IsMuted.ToString());
                Console.WriteLine("Volume =        " + String.Format("{0:0.00}", device.MasterVolume * 100.0));
                Console.WriteLine();

                AudioSession[] sessions = device.GetSessions();

                foreach (AudioSession session in sessions) {
                    Console.WriteLine("[" + device.Id + "/" + session.Id + "/" + session.InstanceId + "]");
                    Console.WriteLine("IsSystemSounds = " + session.IsSystemSoundsSession.ToString());
                    Console.WriteLine("Process ID     = " + session.ProcessId);
                    Console.WriteLine("Display Name   = " + session.DisplayName);
                    Console.WriteLine("Muted          = " + session.IsMuted.ToString());
                    Console.WriteLine("Volume         = " + String.Format("{0:0.00}", session.MasterVolume * 100.0));
                    Console.WriteLine();
                }

                Console.Out.Flush();

                device.MasterVolumeChanged += Device_MasterVolumeChanged;
                device.MuteStatusChanged += Device_MuteStatusChanged;
            }

            Pause();
        }

        private static void Device_MuteStatusChanged(object sender, MuteStatusChangedEventArgs e) {
            if (sender is AudioDevice device) {
                Console.WriteLine("{0} {1}.", device.ToString(), e.MuteStatus ? "muted" : "unmuted");
            }
        }

        private static void Device_MasterVolumeChanged(object sender, VolumeChangedEventArgs e) {
            if (sender is AudioDevice device) {
                Console.WriteLine("{0} volume changed from {1:0.00} to {2:0.00}.", device.ToString(), e.PreviousVolume * 100.0, e.Volume * 100.0);
            }
        }

        static void Pause() {
            Console.Write("Press enter to continue...");
            Console.Out.Flush();
            
            while (Console.ReadKey(true).Key != ConsoleKey.Enter) {

            }

            Console.WriteLine();
        }
    }
}
