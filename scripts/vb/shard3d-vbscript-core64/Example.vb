Imports Shard3D.Core
Imports Shard3D.Core.InternalCalls
Imports Shard3D.Core.Components

Namespace Shard3D.Scripts
    Public Class Example
        Inherits Actor

        Private Sub New()
            Log($"Le id {Me.ID}", LogSeverity.Info)
        End Sub

        Protected Sub BeginEvent()
            Log("BeginEvent() in VB", LogSeverity.Info)
            Log($"ID is {Me.ID}", LogSeverity.Info)
        End Sub

        Protected Sub EndEvent()
            Log("EndEvent() in VB", LogSeverity.Info)
        End Sub

        Protected Sub TickEvent(ByVal dt As Single)
        End Sub

        Protected Sub SpawnEvent()
        End Sub

        Protected Sub KillEvent()
        End Sub
    End Class
End Namespace
