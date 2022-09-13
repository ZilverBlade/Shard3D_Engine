Imports Shard3D.Core

Namespace Shard3D.Scripts
    Public Class PPOManipulator
        Inherits Actor

        Private cameraActor As Actor
        Protected Sub BeginEvent()
            cameraActor = ECS.GetActor("CameraActor")
        End Sub

        Protected Sub EndEvent()

        End Sub

        Protected Sub TickEvent(ByVal dt As Single)
            Dim tick = New Random

            CCPPM.SetPostProcessingParameterValue_float(cameraActor, 2, 1, tick.NextDouble())
        End Sub

        Protected Sub KillEvent()

        End Sub

        Protected Sub SpawnEvent()

        End Sub
    End Class
End Namespace
