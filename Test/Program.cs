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

            AudioDevice[] devices = audioController.GetAudioDevices(DeviceType.All, DeviceState.All);

            foreach (AudioDevice device in devices) {
                Console.WriteLine("[" + device.Id + "]");
                Console.WriteLine("Name =          " + device.Name);
                Console.WriteLine("Friendly Name = " + device.FriendlyName);
                Console.WriteLine("Description =   " + device.Description);
                Console.WriteLine("Type =          " + device.Type.ToString());
                Console.WriteLine("State =         " + device.State.ToString());
                Console.WriteLine("Mute =          " + device.IsMuted.ToString());
                Console.WriteLine();
                Console.Out.Flush();
            }

            Pause();
        }

        static void Pause() {
            Console.Write("Press any key to continue...");
            Console.Out.Flush();
            Console.ReadKey(true);
            Console.WriteLine();
        }
    }
}
