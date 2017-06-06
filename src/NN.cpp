Attribute VB_Name = "Module2"

Dim DX7 As New DirectX7
Dim DSOUND7 As DirectSound
Private SoundBuffer(30) As DirectSoundBuffer
Private useBuffer(15) As DirectSoundBuffer
Private RatBuffer As DirectSoundBuffer
Private BGMBuffer As DirectSoundBuffer
Private bufferdesc As DSBUFFERDESC
Private WavFormat As WAVEFORMATEX
Dim FilePath As String
Dim SoundWave As String // SOUNDWAVE! Jam that transmission!
Dim sCounter As Integer
Dim sFile(30) As String
Dim soundoff As Boolean




Function initSound()

FilePath = App.Path + "\"
Dim j
On Local Error Resume Next
Set DSOUND7 = DX7.DirectSoundCreate("")
If Err.Number Then soundoff = True: Exit Function
DSOUND7.SetCooperativeLevel Form1.hwnd, DSSCL_NORMAL
// SoundWave = soundFile
Set RatBuffer = DSOUND7.CreateSoundBufferFromFile(FilePath + "nothing.wav", bufferdesc, WavFormat)
Set BGMBuffer = DSOUND7.DuplicateSoundBuffer(RatBuffer)
For j = 0 To 20: Set SoundBuffer(j) = DSOUND7.DuplicateSoundBuffer(RatBuffer): Next j


End Function
Function LoadSound(which As Integer, soundFile As String, Optional soundName As String)
If soundoff = True Then Exit Function
SoundWave = soundFile
Set SoundBuffer(which) = DSOUND7.CreateSoundBufferFromFile(FilePath + SoundWave, bufferdesc, WavFormat) // as DirectSoundBuffer

sFile(which) = soundName
If sFile(which) = "" Then sFile(which) = SoundWave

End Function

Function PlaySound(who As String)

Dim which
If soundoff = True Then Exit Function
For j = 0 To 20
If sFile(j) = who Then Exit For
If j = 20 Then j = -1: Exit For
Next j
If j = -1 Then Exit Function
which = j

Let sCounter = sCounter + 1: If sCounter > 15 Then sCounter = 0

Set useBuffer(sCounter) = DSOUND7.DuplicateSoundBuffer(SoundBuffer(which))
useBuffer(sCounter).Stop
useBuffer(sCounter).SetCurrentPosition 0
useBuffer(sCounter).Play DSBPLAY_DEFAULT


// If SoundBuffer(which) = Nothing Then Exit Function
End Function

Function PlaySoundLoop(which As Integer)
If soundoff = True Then Exit Function
// Dim j As Integer
// For j = 0 To 15
// useBuffer(j).GetStatus = DSBSTATUS_PLAYING

Let sCounter = sCounter + 1: If sCounter > 15 Then sCounter = 0


Set useBuffer(sCounter) = DSOUND7.DuplicateSoundBuffer(SoundBuffer(which))
useBuffer(sCounter).Stop
useBuffer(sCounter).SetCurrentPosition 0
useBuffer(sCounter).Play DSBPLAY_LOOPING

// Next j
End Function
Function StopSound(which As Integer)
If soundoff = True Then Exit Function
SoundBuffer(which).Stop
End Function

Function playWave(soundFile As String)
If soundoff = True Then Exit Function

Let sCounter = sCounter + 1: If sCounter > 15 Then sCounter = 0
Set useBuffer(sCounter) = DSOUND7.CreateSoundBufferFromFile(FilePath + soundFile, bufferdesc, WavFormat) // as DirectSoundBuffer
useBuffer(sCounter).Stop
useBuffer(sCounter).SetCurrentPosition 0
useBuffer(sCounter).Play DSBPLAY_DEFAULT

End Function
Function playBGM(soundFile As String)
If soundoff = True Then Exit Function
If soundFile <> "" Then Set BGMBuffer = DSOUND7.CreateSoundBufferFromFile(FilePath + soundFile, bufferdesc, WavFormat) // as DirectSoundBuffer
BGMBuffer.Stop
If soundFile = "" Then Exit Function
BGMBuffer.SetCurrentPosition 0
BGMBuffer.Play DSBPLAY_LOOPING
End Function

Function playIsoWave(soundFile As String)
// The ISOWAVE track no one else can touch
If soundoff = True Then Exit Function

// Let sCounter = sCounter + 1: If sCounter > 15 Then sCounter = 0
Set RatBuffer = DSOUND7.CreateSoundBufferFromFile(FilePath + soundFile, bufferdesc, WavFormat) // as DirectSoundBuffer
RatBuffer.Stop
RatBuffer.SetCurrentPosition 0
RatBuffer.Play DSBPLAY_DEFAULT

End Function

