Namespace Shard3D.Core
    Public Class ECS
        Public Shared Function SpawnActor(Optional ByVal name As String = "Some kind of Actor") As Actor
            Dim guid As ULong = Nothing
            InternalCalls.SpawnActor(guid, name)
            Return New Actor(guid)
        End Function
    End Class
End Namespace