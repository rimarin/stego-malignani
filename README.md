# StegoMalignani
Steganography software for BMP images, based on blue LSB technique; includes cryptography + steganography. Created for the final research paper of high school (2018). 

## Installation
1) Clone the repository
`git clone https://github.com/brancaleon/stegomalignani.git`
2) Compile with command `g++ -o StegoMalignani StegoMalignani.cpp EasyBMP.cpp`
3) Begin to exchange secret messages with your friends 

## Usage
> For **hiding** a message in an image, use: 
`./StegoMalignani -n targetImage.bmp`

Type the secret text, then `<end>`
 
Type the key for encrypting the text
 
> For **revealing** a message from an image, use: 
`./StegoMalignani -r targetImage.bmp`

Type the key for encrypting the text

If the key is correct, you will get the secret message (with a wrong key, you will probably get a nonsense string)

## Best practices
* Never reuse an image and don't allow in any way a comparison between original and edited image.
* Choose strong encryption keys.

* Compressing the image, you will lose the encoded content.

* Try to exchange the image in not-suspect situations, usually posting the photo of your kitty should draw much attention.

## Final notes
It is a quite secure communication system if you are trying to hide something from your colleagues. Feel free to test or contribute to the project, keeping in mind that it is not suited for a professional context.

**Don't trust this software if NSA or governments are tracking you. Go for something a bit more secure.**
