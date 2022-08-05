Imports Shard3D.Core
Imports Shard3D.Core.InternalCalls

Namespace Shard3D.Scripts
    Public Class Example
        Inherits Actor

        Private frames As ULong

        Protected Sub BeginEvent()
            Log("BeginEvent() in C#", LogSeverity.Info)
            Log($"ID is {Me.ID}", LogSeverity.Info)
        End Sub

        Protected Sub EndEvent()
            Log("EndEvent() in C#", LogSeverity.Info)
        End Sub

        Protected Sub TickEvent(ByVal dt As Single)
        End Sub

        Protected Sub KillEvent()
            Log("I died D:", LogSeverity.Info)
            Log($"I lasted {frames} frames")
        End Sub

        Protected Sub SpawnEvent()
        End Sub
    End Class
End Namespace
