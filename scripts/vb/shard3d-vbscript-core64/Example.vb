Imports Shard3D.Core
Imports Shard3D.Core.InternalCalls

Namespace Shard3D.Scripts
    Public Class Example
        Inherits Actor

        Private frames As ULong
        Private elapsed As Single
        Private clr As Vector3

        Protected Sub BeginEvent()
            Log("BeginEvent() in C#", LogSeverity.Info)
            Log($"ID is {Me.ID}", LogSeverity.Info)
            Log($"Do I have a PointlightComponent? {Me.HasComponent(Of Components.PointlightComponent)()}!")
            Log($"Do I have a TransformComponent? {Me.HasComponent(Of Components.TransformComponent)()}!")
            clr = Me.AddComponent(Of Components.PointlightComponent)().Color
            clr.y = 0.0F
            Me.GetComponent(Of Components.PointlightComponent)().Color = clr
            Me.GetComponent(Of Components.TransformComponent)().Translation = New Vector3(0.0F, 0.0F, 1.0F)
        End Sub

        Protected Sub EndEvent()
            Log("EndEvent() in C#", LogSeverity.Info)
        End Sub

        Protected Sub TickEvent(ByVal dt As Single)
            frames += 1
            elapsed += CSng(frames) * dt * 0.005F
            Me.GetComponent(Of Components.PointlightComponent)().Color = New Vector3(CSng(System.Math.Sin(elapsed + 0.5F)), CSng(System.Math.Sin(elapsed + 0.0F)), CSng(System.Math.Sin(elapsed + 0.25F)))
        End Sub

        Protected Sub KillEvent()
            Log("I died D:", LogSeverity.Info)
            Log($"I lasted {frames} frames")
        End Sub

        Protected Sub SpawnEvent()
        End Sub
    End Class
End Namespace
