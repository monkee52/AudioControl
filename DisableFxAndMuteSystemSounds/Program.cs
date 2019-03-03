using AydenIO.AudioControl;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DisableFxAndMuteSystemSounds {
    class Program {
        [STAThread]
        static void Main(string[] args) {
            using (Controller controller = new Controller()) {
                AudioDevice[] devices = controller.GetAudioDevices(DeviceType.Render, DeviceState.Active);

                try {
                    foreach (AudioDevice device in devices) {
                        device.SystemEffectsEnabled = false;

                        AudioSession[] sessions = device.GetSessions();

                        try {
                            foreach (AudioSession session in sessions) {
                                if (session.IsSystemSoundsSession) {
                                    session.Mute();
                                    session.MasterVolume = 0.0f;
                                }
                            }
                        } finally {
                            foreach (AudioSession session in sessions) {
                                ((IDisposable)session).Dispose();
                            }
                        }
                    }
                } finally {
                    foreach (AudioDevice device in devices) {
                        ((IDisposable)device).Dispose();
                    }
                }
            }
        }
    }
}
