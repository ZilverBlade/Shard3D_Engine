Imports Shard3D.Core
Imports Shard3D.Core.InternalCalls

Namespace Shard3D.Scripts
    Public Class Example
        Inherits Actor
        Private frames As ULong
        Private myActor As Actor
        ' Called when level starts playing
        Protected Sub BeginEvent()
            Log("BeginEvent() in VB", LogSeverity.Info)
            Log($"ID is {Me.ID}", LogSeverity.Info)

            myActor = ECS.SpawnActor()
            myActor.AddComponent(Components.PointlightComponent)
            Log($"Spawned in a new actor with GUID {myActor.ID}")
            myActor.Translation = New Vector3(0, -2, 2)
        End Sub

        ' Called when level stops playing
        Protected Sub EndEvent()
            Log("EndEvent() in VB", LogSeverity.Info)
        End Sub

        ' Called every frame, includes frametime value to keep game loop timing accurate
        Protected Sub TickEvent(ByVal dt As Single)
            Dim _T As Vector3 = Me.Translation
            _T.y -= 10.0F * dt
            Me.Translation = _T
            frames += 1

            Dim _T2 As Vector3 = myActor.Translation
            _T2.y += 10.0F * dt
            myActor.Translation = _T2

        End Sub

        ' Called on destruction of the actor
        Protected Sub KillEvent()
            Log("I died D:", LogSeverity.Info)
            Log($"I lasted {frames} frames")
        End Sub

        ' This is unused as there are no blueprints that can be spawned in with scripts
        Protected Sub SpawnEvent()
        End Sub

    End Class
End Namespace
