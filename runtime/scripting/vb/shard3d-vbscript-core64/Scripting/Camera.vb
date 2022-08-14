Imports Shard3D.Core
Imports Shard3D.Core.InternalCalls

Namespace Shard3D.Scripts
    Public Class CameraRotator
        Inherits Actor

        Private camera As Components.CameraComponent
        Private transform As Components.TransformComponent
        Private hasEnded As Boolean

        Protected Sub BeginEvent()
            camera = Me.GetComponent(Of Components.CameraComponent)()
            camera.Possess()
            transform = Me.GetComponent(Of Components.TransformComponent)()
            Dim t = transform.Translation
            Dim r = transform.Rotation
            t.z = 4.0F
            r.x = 35.0F
            transform.Translation = t
            transform.Rotation = r
        End Sub

        Protected Sub EndEvent()
        End Sub

        Protected Sub TickEvent(ByVal dt As Single)
            Dim translation = transform.Translation
            Dim rotation = transform.Rotation

            If hasEnded = False Then
                Dim f = camera.FOV
                f -= 2 * dt
                camera.FOV = f
                translation.z -= 0.2F * dt
                rotation.x -= 1.5F * dt
                If transform.Translation.z < 1.3F Then hasEnded = True
            End If

            transform.Translation = translation
            transform.Rotation = rotation
        End Sub

        Protected Sub KillEvent()
        End Sub

        Protected Sub SpawnEvent()
        End Sub
    End Class
End Namespace
