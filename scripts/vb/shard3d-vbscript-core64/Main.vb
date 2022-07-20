Imports Shard3D.Core


Namespace Shard3D.Scripts
    Public Class Main
        Public Sub New()
            Console.WriteLine("Successfully loaded Visual Basic")
            Dim scale As Vector3 = {1.0F, 5.0F, 5.0F}
            Dim vecthing As Vector2 = {4, 2}
            Dim vec4 As Vector4 = {0, 0, 0, 5}
            Dim newvec As Vector4 = (vec4 + scale) * vecthing * 50

            'expect 4, 10, 5, 5
            InternalCalls.Log($"{newvec.x}, {newvec.y}, {newvec.z}, {newvec.w},")
        End Sub
        Protected Overrides Sub Finalize()
            InternalCalls.Log("Successfully destroyed Visual Basic")
        End Sub
        Public Sub FuncCall(ByRef input As Single)
            InternalCalls.Log($"Visual Basic float: {input}")
        End Sub
    End Class
End Namespace

