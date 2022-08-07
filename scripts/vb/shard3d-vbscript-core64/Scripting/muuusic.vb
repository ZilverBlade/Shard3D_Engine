Imports Shard3D.Core

Namespace Shard3D.Scripts
    Public Class MusicTrack
        Inherits Actor
        Protected Sub BeginEvent()
            Me.AddComponent(Of Components.AudioComponent)().File = "assets/audiodata/thou-3.mp3"
            Me.GetComponent(Of Components.AudioComponent)().Play()
        End Sub

        Protected Sub EndEvent()
            Me.GetComponent(Of Components.AudioComponent)().Stop()
        End Sub

        Protected Sub TickEvent(ByVal dt As Single)

        End Sub

        Protected Sub KillEvent()

        End Sub

        Protected Sub SpawnEvent()

        End Sub
    End Class
End Namespace
