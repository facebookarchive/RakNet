//
// Modal view used to enter the chat server IP/Port
//

#import "ServerDetailsViewController.h"

@implementation ServerDetailsViewController

@synthesize delegate;
@synthesize mServerIP;
@synthesize mServerPort;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

#pragma mark - View lifecycle

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
    } else {
        return YES;
    }
}

-(IBAction)dismissServerDetailsView
{
    // TODO RVF: implement this
    [delegate connectToChatServer:mServerIP.text serverPort:mServerPort.text];
    [self dismissModalViewControllerAnimated:YES];
}

-(void) dealloc
{
    [mServerIP release];
    [mServerPort release];
    
    [super dealloc];
}

@end
