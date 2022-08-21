Imports Shard3D.Core

Namespace Shard3D.Scripts
    Public Class Example
        Inherits Actor

        Private frames As ULong
        Private elapsed As Single

        Protected Sub BeginEvent()
            InternalCalls.Log("BeginEvent() in VB", LogSeverity.Info)
            InternalCalls.Log($"ID is {Me.ID}", LogSeverity.Info)
            InternalCalls.Log($"Do I have a PointlightComponent? {Me.HasComponent(Of Components.PointlightComponent)()}!")
            InternalCalls.Log($"Do I have a TransformComponent? {Me.HasComponent(Of Components.TransformComponent)()}!")
        End Sub

        Protected Sub EndEvent()
            InternalCalls.Log("EndEvent() in VB", LogSeverity.Info)
        End Sub

        Protected Sub TickEvent(ByVal dt As Single)
            frames += 1
            elapsed += dt
        End Sub

        Protected Sub KillEvent()
            InternalCalls.Log("I died D:", LogSeverity.Info)
            InternalCalls.Log($"I lasted {frames} frames, aka I lasted {elapsed / 1000.0F} seconds")
        End Sub

        Protected Sub SpawnEvent()
        End Sub

    End Class
End Namespace
