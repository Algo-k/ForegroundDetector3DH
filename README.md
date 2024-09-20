
ForegroundDetector3DH 
<br>
<div style="text-align: justify;">
This code target is only one class ( CppLibImaging::ForegroundDetector3DH) that conducts some calculations to produce the best "Foreground Color" for a background image or the best "Background Color" for a foreground entity like "Rich Text" or a set of colorful objects. I needed this class thus I created it and this project is still in progress. This class is written in C++ CLR ( C++ .Net ) but you can migrate it to native C++ easily ( No license currently exists ).
Immediately after creating instance of this class you should call "ForegroundDetector3DH::Create" function to initialize the ForegroundDetector3DH instance.
</div>

This code works based on an algorithm based on 3 dimensional histogram.

<div align="center">
    <img src="https://raw.githubusercontent.com/wiki/Algo-k/ForegroundDetector3DH/ThreeDH.jpg" alt="3d histogram" width="20%">
</div>

For more details about code usage or algorithm see [Wiki Page](https://raw.githubusercontent.com/wiki/Algo-k/ForegroundDetector3DH/Home.md)
