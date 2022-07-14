Namespace Shard3D.VBScripts
    Public Class Main
        Public Sub New()
            Console.WriteLine("Successfully loaded Visual Basic")
        End Sub
        Protected Overrides Sub Finalize()
            Console.WriteLine("Successfully destroyed Visual Basic")
        End Sub
        Public Sub FuncCall(ByRef input As Single)
            Console.WriteLine($"Visual Basic float: {input}")
        End Sub
    End Class
End Namespace

