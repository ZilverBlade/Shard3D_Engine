Imports Shard3D.Core
Imports Shard3D.Core.InternalCalls

Namespace Shard3D.Scripts
    Public Class Example
        Inherits Actor

        Private frames As ULong
        Private elapsed As Single

        Protected Sub BeginEvent()
            Log("BeginEvent() in VB", LogSeverity.Info)
            Log($"ID is {Me.ID}", LogSeverity.Info)
            Log($"Do I have a PointlightComponent? {Me.HasComponent(Of Components.PointlightComponent)()}!")
            Log($"Do I have a TransformComponent? {Me.HasComponent(Of Components.TransformComponent)()}!")
        End Sub

        Protected Sub EndEvent()
            Log("EndEvent() in C#", LogSeverity.Info)
        End Sub

        Protected Sub TickEvent(ByVal dt As Single)
            frames += 1
            elapsed += dt
        End Sub

        Protected Sub KillEvent()
            Log("I died D:", LogSeverity.Info)
            Log($"I lasted {frames} frames, aka I lasted {elapsed / 1000.0F} seconds")
        End Sub

        Protected Sub SpawnEvent()
        End Sub
    End Class
End Namespace
