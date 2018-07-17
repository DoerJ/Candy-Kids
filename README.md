# Candy Kids
Utilized producer-consumer solution to manage access to a bounded buffer storing data items

## Course
<dt>2018 Spring, SFU</dt>
<dt>CMPT 300: Operating Systems I</dt>

## Author
James(Yuhao) He

## Platform
Linux & Windows

## Building tools
Standard C 

## License 
This project is under C License

## Overview
### Inputs
<dt>The program will accept three arguments: </dt>
<dt> ./candykids <# of factories> <# of kids> <# of seconds> </dt>
</br>
<dt># of factories: Number of factory threads to spawn</dt>
<dt># of kids: Number of kids threads to spawn</dt>
<dt># of seconds: The number of seconds to allow the factory threads to run for</dt>

### Outputs
<dt>Display a statistics module tracking:</dt>
</br>
<dt>Count the number of candies each factory has produced</dt>
<dt>Count the number fo candies that were consumed from each factory</dt>
<dt>For each factory, the min, max, and average delays for how long it took from the moment the candy was produced (dynamically allocated) until consumed (eaten by the kid).</dt>

### Sample stats summary
<img width="422" alt="screen shot 2018-05-01 at 9 51 46 pm" src="https://user-images.githubusercontent.com/30460622/39501608-e36e1762-4d89-11e8-94b9-31fb1b0123b0.png">
